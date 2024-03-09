/*
   The table is designed to allow you to include it multiple times to "pull out" other things
   from the table, such as html tagname as strings or enums, if a tag has a closing tag etc.
*/
#ifndef STRLEN
  #define STRLEN(X) (U64)(sizeof(X) - 1)
#endif
#ifndef STR
  #define STR(x) #x
#endif

#ifndef STR_WITH_TAGS_WITH_CLOSING
  #define STR_WITH_TAGS_WITH_CLOSING(x) STR(<##x></##x>)
#endif

#ifndef STR_WITH_SINGLE_TAG
  #define STR_WITH_SINGLE_TAG(x) STR(<##x>)
#endif

#ifndef STR_WITH_TAGS
  #define STR_WITH_TAGS(x, isSingle)((isSingle)?\
       STR_WITH_SINGLE_TAG(x),STRLEN(STR_WITH_SINGLE_TAG(x)):\
       STR_WITH_TAGS_WITH_CLOSING(x),STRLEN(STR_WITH_TAGS_WITH_CLOSING(x)))
#endif    


#ifndef TAG
  #define TAG(Tag, ClosingType, ...) { HTMLTag_##Tag, HTMLTagClosingType_##ClosingType, __VA_ARGS__ },
#endif

#ifndef true
  #define true 1
#endif
#ifndef false
  #define false 0
#endif

#define A          { (U8*)"a",         sizeof("a")-1 }
#define P          { (U8*)"p",         sizeof("p")-1 }
#define LI         { (U8*)"li",        sizeof("li")-1 }
#define UL         { (U8*)"ul",        sizeof("ul")-1 }
#define OL         { (U8*)"ol",        sizeof("ol")-1 }
#define H1         { (U8*)"h1",        sizeof("h1")-1 }
#define H2         { (U8*)"h2",        sizeof("h2")-1 }
#define H3         { (U8*)"h3",        sizeof("h3")-1 }
#define H4         { (U8*)"h4",        sizeof("h4")-1 }
#define DIV        { (U8*)"div",       sizeof("div")-1 }
#define IMG        { (U8*)"img",       sizeof("img")-1 }
#define BR         { (U8*)"br",        sizeof("br")-1 }
#define STRONG     { (U8*)"strong",    sizeof("strong")-1 }
#define SPAN       { (U8*)"span",      sizeof("span")-1 }
#define PRE        { (U8*)"pre",       sizeof("pre")-1 }
#define U          { (U8*)"u",         sizeof("u")-1 }
#define EM         { (U8*)"em",        sizeof("em")-1 }
#define CODE       { (U8*)"code",      sizeof("code")-1 }
#define SECTION    { (U8*)"section",   sizeof("section")-1 }
#define META       { (U8*)"meta",      sizeof("meta")-1 }
#define LINK       { (U8*)"link",      sizeof("link")-1 }
#define SCRIPT     { (U8*)"script",    sizeof("script")-1 }
#define SMALL      { (U8*)"small",     sizeof("small")-1 }
#define ARTICLE    { (U8*)"article",   sizeof("article")-1 }
#define NAV        { (U8*)"nav",       sizeof("nav")-1 }
#define HEADER     { (U8*)"header",    sizeof("header")-1 }
#define MAIN       { (U8*)"main",      sizeof("main")-1 }
#define ASIDE      { (U8*)"aside",     sizeof("aside")-1 }
#define TABLE      { (U8*)"table",     sizeof("table")-1 }
#define TR         { (U8*)"tr",        sizeof("tr")-1 }
#define TH         { (U8*)"th",        sizeof("th")-1 }
#define TD         { (U8*)"td",        sizeof("td")-1 }
#define FORM       { (U8*)"form",      sizeof("form")-1 }
#define INPUT      { (U8*)"input",     sizeof("input")-1 }
#define BUTTON     { (U8*)"button",    sizeof("button")-1 }
#define HEAD       { (U8*)"head",      sizeof("head")-1 }
#define DOCTYPE    { (U8*)"!DOCTYPE",  sizeof("!DOCTYPE")-1 }
#define HTML       { (U8*)"html",      sizeof("html")-1 }
#define TITLE      { (U8*)"title",     sizeof("title")-1 }
#define BODY       { (U8*)"body",      sizeof("body")-1 }
#define FOOTER     { (U8*)"footer",    sizeof("footer")-1 }
#define LABEL      { (U8*)"label",     sizeof("label")-1 }
TAG(a,      Paired,   A)
TAG(p,      Paired,   P)
TAG(li,     Paired,   LI)
TAG(ul,     Paired,   UL)
TAG(ol,     Paired,   OL)
TAG(h1,     Paired,   H1)
TAG(h2,     Paired,   H2)
TAG(h3,     Paired,   H3)
TAG(h4,     Paired,   H4)
TAG(div,    Paired,   DIV)
TAG(img,    Unique,   IMG)
TAG(br,     Unique,   BR)
TAG(strong, Paired,   STRONG)
TAG(span,   Paired,   SPAN)
TAG(pre,    Paired,   PRE)
TAG(u,      Paired,   U)
TAG(em,     Paired,   EM)
TAG(code,   Paired,   CODE)
TAG(section,Paired,   SECTION)
TAG(meta,   Unique,   META)
TAG(link,   Unique,   LINK)
TAG(script, Paired,   SCRIPT)
TAG(small,  Paired,   SMALL)
TAG(article,Paired,   ARTICLE)
TAG(nav,    Paired,   NAV)
TAG(header, Paired,   HEADER)
TAG(main,   Paired,   MAIN)
TAG(aside,  Paired,   ASIDE)
TAG(table,  Paired,   TABLE)
TAG(tr,     Paired,   TR)
TAG(th,     Paired,   TH)
TAG(td,     Paired,   TD)
TAG(form,   Paired,   FORM)
TAG(input,  Unique,   INPUT)
TAG(button, Paired,   BUTTON)
TAG(label,  Paired,   LABEL)
TAG(head,   Paired,   HEAD)
TAG(doctype,Unique,   DOCTYPE)
TAG(html,   Paired,   HTML)
TAG(title,  Paired,   TITLE)
TAG(body,   Paired,   BODY)
TAG(footer, Paired,   FOOTER)

#undef A
#undef P
#undef LI
#undef UL
#undef OL
#undef H1
#undef H2
#undef H3
#undef H4
#undef DIV
#undef IMG
#undef BR
#undef STRONG
#undef SPAN
#undef PRE
#undef U
#undef EM
#undef CODE
#undef SECTION
#undef META
#undef LINK
#undef SCRIPT
#undef SMALL
#undef ARTICLE
#undef NAV
#undef HEADER
#undef MAIN
#undef ASIDE
#undef TABLE
#undef TR
#undef TH
#undef TD
#undef FORM
#undef INPUT
#undef BUTTON
#undef LABEL
#undef HEAD
#undef DOCTYPE
#undef HTML
#undef TITLE
#undef BODY
#undef FOOTER

#undef TAG