void frag_pass()
{
  set_frag(vec4(get_world_pos(), 1.0));
  // set_frag(vec4(get_frag_pos() * 0.25, 1.0));
}
