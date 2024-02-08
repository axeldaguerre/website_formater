internal HtmlTokenNode*
html_token_push_list(Arena *arena, HtmlTokenList *list, HtmlToken token)
{
  HtmlTokenNode *node = push_array_no_zero(arena, HtmlTokenNode, 1);
  SLLPush(list->first, list->last, node);
  list->count += 1;
  
  return node;
}