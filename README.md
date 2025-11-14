# Non UDP HTTP

<h4 align="center">Non UDP HTTP is a simple HTTP server implementation in C</h4>

<br />
<br />


## TODO:
- [ ] Adding support for parsing query parameters from the URL
- [ ] Adding support for defining routes (e.g., GET /home) and their handlers
- [ ] Adding code to construct responses
- [ ] Implementing concurrent request handling using threads or asynchronous I/O
- [ ] Adding support for middleware functions that can process requests before they reach the route handlers
- [ ] Adding support for serving static files (e.g., HTML, CSS, JS)
- [ ] Adding support for parsing and constructing request bodies (e.g., JSON)



# Query params parsing

  According to RFC 3986 (URI syntax), these are all valid:

  1. Empty Value

  ?key=
  - Key: key
  - Value: empty string

  2. No Equals Sign

  ?key
  - Key: key
  - Value: Could be interpreted as empty, or as "flag" (no value)

  3. Empty Key

  ?=value
  - Key: empty string
  - Value: value

  4. Completely Empty Parameter

  ?&
  or
  ?key1=val1&&key2=val2
  Empty parameter between delimiters

  5. Just the Question Mark

  GET /?  HTTP/1.1
  Query string exists but contains no parameters

  Parsing Challenges

  When implementing your parser, you need to decide:

  1. ?key vs ?key= - Are these the same or different?
    - ?key might mean a boolean flag
    - ?key= explicitly has an empty value
  2. How to handle && - Skip it? Store empty param? Error?
  3. What about ?=value - Is an empty key valid for your use case?
  4. Edge case: ? with nothing after - is this "no parameters" or "empty parameter"?
