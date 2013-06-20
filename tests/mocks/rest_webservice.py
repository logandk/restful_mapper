from flask import Flask, Response
from flask import request
from flask.ext import restless
from flask.ext import sqlalchemy

# Custom column type, enforcing UTC
from sqlalchemy import types
from dateutil.tz import tzutc
from datetime import datetime
from dateutil.parser import parse as parse_datetime

class UTCDateTime(types.TypeDecorator):
    impl = types.DateTime

    def process_bind_param(self, value, engine):
        if isinstance(value, str) or isinstance(value, unicode):
            if value.strip() == '':
                value = None
            else:
                value = parse_datetime(value).replace(tzinfo=tzutc())

        if value is not None:
            return value.astimezone(tzutc())

    def process_result_value(self, value, engine):
        if value is not None:
            return datetime(value.year, value.month, value.day,
                            value.hour, value.minute, value.second,
                            value.microsecond, tzinfo=tzutc())

class ValidationError(Exception):
    def __init__(self, field, message):
        Exception.__init__(self, message)
        self.errors = { field: message }


# Initialize the app
app = Flask(__name__)
db  = sqlalchemy.SQLAlchemy(app)
api = restless.APIManager(app, flask_sqlalchemy_db=db)

# Database configuration
app.config['DEBUG'] = True
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite://'
#app.config['SQLALCHEMY_ECHO'] = True

# Monkeypatches
# Disable pagination
#restless.views.API._paginated = lambda self, instances, deep: dict(objects=[restless.views._to_dict(x, deep) for x in instances])

# Debugging
@app.after_request
def debug_response_callback(response):
    print response.data
    return response

@app.before_request
def debug_request_callback():
    print request, request.headers, request.json

@app.before_request
def auth_request_callback():
    if request.path == '/api/reload':
        return

    auth = request.authorization

    if not auth or auth.username != 'admin' or auth.password != 'test':
        return Response(status=401, headers={'WWW-Authenticate': 'Basic realm="mock"'})

# Define data models
class Todo(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    task = db.Column(db.Unicode)
    priority = db.Column(db.Integer)
    time = db.Column(db.Float)
    completed = db.Column(db.Boolean)
    completed_on = db.Column(UTCDateTime)

class Country(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.Unicode)

class City(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    country_id = db.Column(db.Integer, db.ForeignKey('country.id'))
    name = db.Column(db.Unicode)

    country = db.relationship('Country', backref=db.backref('cities', lazy='dynamic'))

class Zipcode(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    city_id = db.Column(db.Integer, db.ForeignKey('city.id'))
    code = db.Column(db.Unicode)

    city = db.relationship('City', backref=db.backref('zipcode', uselist=False))

    @db.validates('code')
    def validate_code(self, key, value):
        if not len(value) == 4: raise ValidationError(key, 'must have 4 digits')
        return value

class Citizen(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    country_id = db.Column(db.Integer, db.ForeignKey('country.id'))
    city_id = db.Column(db.Integer, db.ForeignKey('city.id'))
    first_name = db.Column(db.Unicode)
    last_name = db.Column(db.Unicode)

    country = db.relationship('Country', backref=db.backref('citizens', lazy='dynamic'))
    city = db.relationship('City', backref=db.backref('citizens', lazy='dynamic'))

class PhoneNumber(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    citizen_id = db.Column(db.Integer, db.ForeignKey('citizen.id'))
    number = db.Column(db.Integer)

    citizen = db.relationship('Citizen', backref=db.backref('phone_numbers', lazy='dynamic'))

# Seed database
def seed_db():
    db.drop_all()
    db.create_all()

    todos = [Todo(task=u'Build an API', priority=9, time=10.35, completed=True, completed_on=datetime(2013, 12, 5).replace(tzinfo=tzutc())),
        Todo(task=u'???', priority=2, time=4.54, completed=False, completed_on=datetime(2013, 03, 13, 11, 53, 21, 80000).replace(tzinfo=tzutc())),
        Todo(task=u'Profit!!!', priority=994, time=0.001, completed=False, completed_on=datetime(2013, 12, 5).replace(tzinfo=tzutc()))]

    for todo in todos:
        db.session.add(todo)

    country = Country(name=u'Denmark')

    city = City(name=u'Copenhagen', zipcode=Zipcode(code=u'1200'), country=country)
    Citizen(first_name=u'John', last_name=u'Doe', phone_numbers=[PhoneNumber(number=1234),PhoneNumber(number=5678)], city=city)
    Citizen(first_name=u'Jane', last_name=u'Doe', phone_numbers=[PhoneNumber(number=865865)], city=city)

    city = City(name=u'Aarhus', zipcode=Zipcode(code=u'8000'), country=country)
    Citizen(first_name=u'Bob', last_name=u'Doe', phone_numbers=[PhoneNumber(number=999),PhoneNumber(number=58),PhoneNumber(number=1234)], city=city)

    db.session.add(country)

    country = Country(name=u'Sweden')

    city = City(name=u'Stockholm', country=country)
    Citizen(first_name=u'Alfons', last_name=u'Aaberg', city=city)

    db.session.add(country)

    country = Country(name=u'Norway')
    db.session.add(country)

    db.session.commit()

seed_db()

# Define API
api.create_api(Todo, methods=['GET', 'POST', 'DELETE', 'PUT'], results_per_page=None, validation_exceptions=[ValidationError])
api.create_api(Country, methods=['GET', 'POST', 'DELETE', 'PUT'], results_per_page=None, validation_exceptions=[ValidationError])
api.create_api(City, methods=['GET', 'POST', 'DELETE', 'PUT'], results_per_page=None, validation_exceptions=[ValidationError])
api.create_api(Zipcode, methods=['GET', 'POST', 'DELETE', 'PUT'], results_per_page=None, validation_exceptions=[ValidationError])
api.create_api(Citizen, methods=['GET', 'POST', 'DELETE', 'PUT'], results_per_page=None, validation_exceptions=[ValidationError])
api.create_api(PhoneNumber, methods=['GET', 'POST', 'DELETE', 'PUT'], results_per_page=None, validation_exceptions=[ValidationError])

# Reloader
@app.route("/api/reload")
def reload_db():
    seed_db()
    return "Done!"

if __name__ == '__main__':
    app.run(debug=True)

