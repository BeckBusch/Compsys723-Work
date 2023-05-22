//The type 'string' is defined as 'char*' in Esterel, hence passing a string by referenc  is 'char**'
//Aviod naming a function 'recv'
void send(int data);
void recv1(char** data);
void adding(unsigned int data);
int remov();
int outCount();
