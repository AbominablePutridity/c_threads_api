#ifndef JWT_H
#define JWT_H

char* generate_token(const char* user_login);
int   validate_token(const char* token);

int jwt_usability_example(void);

#endif