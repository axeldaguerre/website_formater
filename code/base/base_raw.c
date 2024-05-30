internal B32
RAW_IsNil(RawDataNode *raw)
{
  return raw == 0 || raw == &raw_node_g_nil;
}
