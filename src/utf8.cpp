#include <restful_mapper/internal/utf8.h>
#include <sstream>
#include <iconv.h>
#include <errno.h>

using namespace std;

string local_charset = "";

string iconv_string(const string &value, const char *to, const char *from)
{
  string out;

  // Prepare source buffers
  size_t src_len = value.size();
  char *src      = new char[src_len];
  value.copy(src, src_len);

  // Prepare destination buffer
  size_t buf_len = src_len + 3;
  size_t dst_len = buf_len;
  char *dst      = new char[buf_len];

  // Initialize iconv
  iconv_t conv = iconv_open(to, from);

  if (conv == (iconv_t)(-1))
  {
    ostringstream s;
    s << "Unable to initialize iconv (error " << errno << ")";
    throw runtime_error(s.str());
  }

  // Perform conversion
  char *src_ptr       = src;
  char *dst_ptr       = dst;
  size_t *src_len_ptr = &src_len;
  size_t *dst_len_ptr = &dst_len;

  while (src_len)
  {
    size_t status = iconv(conv, &src_ptr, src_len_ptr, &dst_ptr, dst_len_ptr);

    if (status == (size_t)(-1))
    {
      if (errno == E2BIG)
      {
        // Flush to output string
        out.append(dst, buf_len - dst_len);

        dst_ptr = dst;
        dst_len = buf_len;
      }
      else
      {
        iconv_close(conv);

        ostringstream s;
        s << "Unable to convert string to " << to << " (error " << errno << "): " << src;
        throw runtime_error(s.str());
      }
    }
    else
    {
      // Flush to output string
      out.append(dst, buf_len - dst_len);
    }
  }

  iconv_close(conv);

  // Copy to string object
  delete[] src;
  delete[] dst;

  return out;
}

string local_to_utf8(const string &value)
{
  return iconv_string(value, "UTF-8", local_charset.c_str());
}

string utf8_to_local(const string &value)
{
  return iconv_string(value, local_charset.c_str(), "UTF-8");
}
