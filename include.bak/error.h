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

  static constexpr Status FAILURE = {.state = FAILURE_E};
  static constexpr Status SUCCESS = {.state = SUCCESS_E};
  static constexpr Status NOT_EXECUTE = {.state = NOT_EXECUTE_E};

#endif
