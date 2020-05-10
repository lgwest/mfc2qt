class Car
{
  public:
    Car();
    Car(int iSpeed, int iDirection);
    ~Car();

    void IncreaseSpeed(int iSpeed);
    void DecreaseSpeed(int iSpeed);

    void TurnLeft(int iAngle);
    void TurnRight(int iAngle);

    int GetSpeed();
    int GetDirection();

  private:
    int m_iSpeed, m_iDirection;
};
