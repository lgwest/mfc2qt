template<typename T>
class List : public CList<T>
{
  public:
    List();
    List(const List<T>& list);

    void Remove(T value);
    List<T> FilterIf(BOOL Predicate(T value)) const;
    int CountIf(BOOL Predicate(T value)) const;
};

template<typename T>
List<T>::List()
{
  // Empty.
}

template<typename T>
List<T>::List(const List<T>& list)
{
  for (POSITION position = list.GetHeadPosition();
       position != NULL; list.GetNext(position))
  {
    T value = list.GetAt(position);
    AddTail(value);
  }
}

template<typename T>
void List<T>::Remove(T value)
{
  POSITION position = Find(value);

  if (position != NULL)
  {
    RemoveAt(position);
  }
}

template<typename T>
List<T> List<T>::FilterIf(BOOL Predicate(T value)) const
{
  List<T> result;

  for (POSITION position = GetHeadPosition();
       position != NULL; GetNext(position))
  {
    T value = GetAt(position);

    if (Predicate(value))
    {
      result.AddTail(value);
    }
  }

  return result;
}

template<typename T>
int List<T>::CountIf(BOOL Predicate(T value)) const
{
  int iCount = 0;

  for (POSITION position = GetHeadPosition();
       position != NULL; GetNext(position))
  {
    T value = GetAt(position);

    if (Predicate(value))
    {
      ++iCount;
    }
  }

  return iCount;
}

/*
template<typename T>
List<T> List<T>::FilterIs(Figure* BOOL Predicate(T value)) const
{
  List<T> result;

  for (POSITION position = GetHeadPosition();
       position != NULL; GetNext(position))
  {
    T value = GetAt(position);

    if (Predicate(value))
    {
      result.AddTail(value);
    }
  }

  return result;
}
*/