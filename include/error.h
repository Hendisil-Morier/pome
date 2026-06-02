#ifndef ERROR_H
#define ERROR_H
  typedef struct
  {
    enum
    {
      SUCCESS_E,
      FAILURE_E,
      NOT_EXECUTE_E
    } state;
  } Status;

  #define FAILED(s) ((s).state == FAILURE_E)
  #define SUCCEEDED(s) ((s).state == SUCCESS_E)

  #define SUCCESS ((Status){SUCCESS_E})
  #define FAILURE ((Status){FAILURE_E})
  #define NOT_EXECUTE ((Status){NOT_EXECUTE_E})

#endif
