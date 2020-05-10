template<typename T>
class Set : public CList<T>
{
  public:
    Set();
    Set(const Set<T>& set);
    Set<T>& operator=(const Set<T>& set);

    void Add(T value);
    void AddAll(Set<T>& set);

    void Remove(T value);
    BOOL Exists(T value) const;

    static Set<T> Merge(Set<T> leftSet, Set<T> rightSet,
                        BOOL bAddEQ, BOOL bAddLT,BOOL bAddGT,
                        BOOL bAddLeft, BOOL bAddRight);

    static Set<T> Union(Set<T> leftSet, Set<T> rightSet);
    static Set<T> Intersection(Set<T> leftSet,
                               Set<T> rightSet);
    static Set<T> Difference(Set<T> leftSet, Set<T> rightSet);
    static Set<T> SymmetricDifference(Set<T> leftSet,
                                      Set<T> rightSet);
};

template<typename T>
Set<T>::Set()
{
  // Empty.
}

template<typename T>
Set<T>::Set(const Set<T>& set)
{
  operator=(set);
}

template<typename T>
Set<T>& Set<T>::operator=(const Set<T>& set)
{
  RemoveAll();

  for (POSITION position = set.GetHeadPosition();
       position != NULL; set.GetNext(position))
  {
    T value = set.GetAt(position);
    AddTail(value);
  }

  return *this;
}

template<typename T>
void Set<T>::Add(T newValue)
{
  for (POSITION position = GetHeadPosition();
       position != NULL; GetNext(position))
  {
    T value = GetAt(position);

    if (value == newValue)
    {
      return;
    }

    else if (newValue < value)
    {
      InsertBefore(position, newValue);
      return;
    }
  }

  AddTail(newValue);
}

template<typename T>
void Set<T>::AddAll(Set<T>& set)
{
  for (POSITION position = set.GetHeadPosition();
       position != NULL; set.GetNext(position))
  {
    T value = set.GetAt(position);
    Add(value);
  }
}

template<typename T>
void Set<T>::Remove(T value)
{
  POSITION position = Find(value);

  if (position != NULL)
  {
    RemoveAt(position);
  }
}

template<typename T>
BOOL Set<T>::Exists(T value) const
{
  POSITION position = Find(value);
  return (position != NULL);
}

template<typename T>
Set<T> Set<T>::Merge(Set<T> leftSet, Set<T> rightSet,
                     BOOL bAddEQ, BOOL bAddLT, BOOL bAddGT,
                     BOOL bAddLeftRest, BOOL bAddRightRest)
{
  Set<T> resultSet;

  while (!leftSet.IsEmpty() && !rightSet.IsEmpty())
  {
    T leftValue = leftSet.GetHead();
    T rightValue = rightSet.GetHead();

    if (leftValue == rightValue)
    {
      if (bAddEQ)
      {
        resultSet.AddTail(leftValue);
      }

      leftSet.RemoveHead();
      rightSet.RemoveHead();
    }

    else if (leftValue < rightValue)
    {
      if (bAddLT)
      {
        resultSet.AddTail(leftValue);
      }

      leftSet.RemoveHead();
    }

    else
    {
      if (bAddGT)
      {
        resultSet.AddTail(rightValue);
      }

      rightSet.RemoveHead();
    }
  }

  if (bAddLeftRest)
  {
    resultSet.AddAll(leftSet);
  }

  if (bAddRightRest)
  {
    resultSet.AddAll(rightSet);
  }

  return resultSet;
}

template<typename T>
Set<T> Set<T>::Union(Set<T> leftSet, Set<T> rightSet)
{
  return Merge(leftSet, rightSet, TRUE, TRUE, TRUE, TRUE, TRUE);
}

template<typename T>
Set<T> Set<T>::Intersection(Set<T> leftSet,
                                  Set<T> rightSet)
{
  return Merge(leftSet, rightSet, TRUE, FALSE, FALSE, FALSE, FALSE);
}

template<typename T>
Set<T> Set<T>::Difference(Set<T> leftSet, Set<T> rightSet)
{
  return Merge(leftSet, rightSet, FALSE, TRUE, FALSE, TRUE, FALSE);
}

template<typename T>
Set<T> Set<T>::SymmetricDifference(Set<T> leftSet,
                                         Set<T> rightSet)
{
  return Merge(leftSet, rightSet, FALSE, TRUE, TRUE, TRUE, TRUE);
}
