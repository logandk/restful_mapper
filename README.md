# restful_mapper #

ORM for consuming RESTful APIs in C++

[![Build status](https://secure.travis-ci.org/logandk/restful_mapper.png)](http://travis-ci.org/logandk/restful_mapper)

# Introduction #

**restful\_mapper** connects business objects and Representational State
Transfer (REST) web services. It implements object-relational mapping for REST
web services to provide transparent proxying capabilities between a client
(using **restful_mapper**) and a RESTful web service (that follows the
conventions outlined below).

The ideas and design philosophy are directly adopted from [Active Resource][3].
However, the API and functionality differ in some areas, where it makes sense.

## Design goals ##

* _Simple_: Employ the principle of least surprise -- APIs should be clean and
  intuitive
* _Portable_: Support old compilers -- all code is C++03 and C
* _Robust_: Keep the code base small -- rely on proven and stable libraries to
  provide core functionality
* _Internationalized_: Handle timezones and character sets automatically

## RESTful web service conventions ##

RESTful web services come in many different forms, which are not all suitable
for an ORM-style mapper such as **restful_mapper**. In order to be compatible
with as many web services as possible, **restful_mapper** complies with the
current best-practice for creating RESTful web services with JSON.

Specifically, **restful_mapper** is modelled against the [Flask-Restless][4]
library, which provides full-featured RESTful web services that follow best-
practice design methods.

The following basic conventions must be followed by the web service:

* Uses standard `DELETE`, `GET`, `POST` and `PUT` requests for CRUD-operations
* Collection of objects should use `objects` as the root JSON key
* Objects in a relationship should be represented as nested JSON structures
* If authentication is used, it must be HTTP basic authentication

For exact details on expected request and response formats, see [Format of requests and responses][5].

# Building #

**restful\_mapper** is built using [CMake][6].

## Dependencies ##

The following libraries are required to build **restful_mapper**.

* [libcurl][7] - used for comminucating with the web service over HTTP
* [yajl][8] - used to parse and emit JSON
* [libiconv][9] - used to convert between character sets
* [googletest][10] - required for tests only

Invoking the following command will download and build these libraries.

```shell
make vendor
```

On UNIX platforms, [libcurl][7] and [libiconv][9] are typically present on the
system, and will not be built by the make command. If they are not present,
they should be installed using the system package manager.

## Library ##

After building the dependencies, invoke the following command to build **restful_mapper**.

```shell
make
```

This will install **restful_mapper** as a static library in the `lib` folder.

## Tests ##

The test suite can be built and run using the following command.

```shell
make test
```

# Usage #

## API configuration ##

Before making any requests to the web service, it must be configured using the
following methods.

The root URL of the web service is specified using the `set_url` method:

```c++
Api::set_url("http://localhost:5000/api");
```

If the web service requires authentication, provide the username and password:

```c++
Api::set_username("admin");
Api::set_password("test");
```

If you are using a proxy server, it can be specified through the `HTTP_PROXY`
environment variable or using the `set_proxy` method:

```c++
Api::set_proxy("http://myproxy");
```

## Mapper configuration ##

This example illustrates a complete object mapping:

```c++
using namespace std;
using namespace restful_mapper;

class User;

class Todo : public Model<Todo>
{
public:
  Primary id;
  Field<string> task;
  Field<int> priority;
  Field<double> time;
  Field<bool> completed;
  Field<time_t> completed_on;
  Field<int> user_id;
  HasOne<User> user;

  virtual void map_set(Mapper &mapper) const
  {
    mapper.set("id", id);
    mapper.set("task", task);
    mapper.set("priority", priority);
    mapper.set("time", time);
    mapper.set("completed", completed);
    mapper.set("completed_on", completed_on);
    mapper.set("user_id", user_id);
    mapper.set("user", user);
  }

  virtual void map_get(const Mapper &mapper)
  {
    mapper.get("id", id);
    mapper.get("task", task);
    mapper.get("priority", priority);
    mapper.get("time", time);
    mapper.get("completed", completed);
    mapper.get("completed_on", completed_on);
    mapper.get("user_id", user_id);
    mapper.get("user", user);
  }

  virtual std::string endpoint() const
  {
    return "/todo";
  }

  virtual Primary &primary()
  {
    return id;
  }
};

class User: public Model<User>
{
public:
  Primary id;
  HasMany<Todo> todos;
  ...
};
```

An API entity is declared by creating a class that inherits from and follows the
interface defined in `restful_mapper::Model`.

Each entity can hold a number of fields and relationships:

* `restful_mapper::Primary` -- maps a primary key (integer)
* `restful_mapper::Field<string>` -- maps a string literal (represented in locale charset)
* `restful_mapper::Field<int>` -- maps an integer value
* `restful_mapper::Field<double>` -- maps a floating point value
* `restful_mapper::Field<bool>` -- maps a boolean value
* `restful_mapper::Field<time_t>` -- maps a datetime value (represented in the local timezone)
* `restful_mapper::HasOne<class>` -- maps a one-to-one or many-to-one relationship
* `restful_mapper::HasMany<class>` -- maps a one-to-many relationship

The interface specified the following methods, which must be overriden:

* `virtual void restful_mapper::Model::map_set(Mapper &mapper) const`
* `virtual void restful_mapper::Model::map_get(const Mapper &mapper)`
* `virtual std::string restful_mapper::Model::endpoint() const`
* `virtual Primary &restful_mapper::Model::primary()`

## Working with objects ##

# Contributing #

Pull requests on [GitHub][1] are very welcome! Please try to follow these simple rules:

* Please create a topic branch for every separate change you make.
* Make sure your patches are well tested. All tests must pass on [Travis CI][2].
* Update this [`README.md`](http://github.com/logandk/restful_mapper/README.md) if applicable.

# License #

This code is copyright 2013 Logan Raarup, and is released under the revised BSD License.

For more information, see [`LICENSE`](http://github.com/logandk/restful_mapper/LICENSE).


[1]: http://github.com/logandk/restful_mapper
[2]: http://travis-ci.org/logandk/restful_mapper
[3]: https://github.com/rails/activeresource
[4]: https://github.com/jfinkels/flask-restless
[5]: https://flask-restless.readthedocs.org/en/latest/requestformat.html
[6]: http://www.cmake.org
[7]: http://curl.haxx.se/libcurl
[8]: http://lloyd.github.io/yajl
[9]: http://www.gnu.org/software/libiconv
[10]: https://code.google.com/p/googletest
