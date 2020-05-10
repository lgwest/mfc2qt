class Parser
{
  public:
    SyntaxTree Formula(const CString& stBuffer);

  private:
    void Match(TokenIdentity eTokenId);

    SyntaxTree* Expression();
    SyntaxTree* NextExpression(SyntaxTree* pLeftTerm);
    SyntaxTree* Term();
    SyntaxTree* NextTerm(SyntaxTree* pLeftFactor);
    SyntaxTree* Factor();

  private:
    CString m_stBuffer;
    Token m_nextToken;
    TokenList* m_pTokenList;
};
