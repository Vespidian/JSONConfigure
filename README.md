# JSONConfigure
Simple, quick, and easy JSON Library for reading any JSON files

#### [Example Usage](src/tests/example.c) - See also [all the test files](src/tests/)

#### Notes
- When using 'JSONTokenToString' make sure you eventually free() the string
- When using a 'JSONToken' you should only access the type which 'JSONToken.type' specifies
- Dictionaries passed to 'JSONTokenHash' **must** be NULL terminated
- Some variables in 'JSONState' are only used internally
