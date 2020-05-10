#define check(test)                                            \
{                                                              \
  if (!(test))                                                 \
  {                                                            \
    CString stMessage;                                         \
    stMessage.Format(TEXT("\"%s\" on line %d in file %s"),     \
                     TEXT(#test), __LINE__, TEXT(__FILE__));   \
    ::MessageBox(NULL, stMessage, TEXT("Error Check"), MB_OK); \
    ::exit(-1);                                                \
  }                                                            \
}

#define check_memory(alloc_code)                            \
{                                                           \
  try                                                       \
  {                                                         \
    alloc_code;                                             \
  }                                                         \
  catch (CException*)                                       \
  {                                                         \
    CString stMessage;                                      \
    stMessage.Format(TEXT("Out of memory \"%s\" ")          \
                     TEXT("on line %d in file %s"),         \
                     TEXT(#alloc_code), __LINE__,           \
                     TEXT(__FILE__));                       \
    ::MessageBox(NULL,stMessage,TEXT("Memory Check"),MB_OK);\
    ::exit(-1);                                             \
  }                                                         \
}
