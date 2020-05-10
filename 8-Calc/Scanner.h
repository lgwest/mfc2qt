class Scanner
{
  public:
    Scanner(const CString& stBuffer);
    TokenList* GetTokenList() {return &m_tokenList;}

  private:
    Token NextToken();

    BOOL ScanValue(double& dValue);
    BOOL ScanReference(Reference& reference);

    CString ScanSign();
    CString ScanDigits();

    CString m_stBuffer;
    TokenList m_tokenList;
};
