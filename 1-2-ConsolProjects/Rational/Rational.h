class Rational
{
  public:
    Rational(int iNumerator = 0, int iDenumerator = 1);

    Rational(const Rational& rational);
    Rational operator=(const Rational& rational);

    bool operator==(const Rational &number) const;
    bool operator!=(const Rational &number) const;
    bool operator< (const Rational &number) const;
    bool operator<=(const Rational &number) const;
    bool operator> (const Rational &number) const;
    bool operator>=(const Rational &number) const;

    Rational operator+(const Rational &number) const;
    Rational operator-(const Rational &number) const;
    Rational operator*(const Rational &number) const;
    Rational operator/(const Rational &number) const;

    friend istream &operator>>(istream &inputStream, Rational &number);
    friend ostream &operator<<(ostream &outputStream, const Rational &number);

  private:
    int m_iNumerator, m_iDenumerator;

    void Normalize();
    int GreatestCommonDivider(int iNum1, int iNum2);
};
