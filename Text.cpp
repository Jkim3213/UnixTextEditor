#include "Text.h"

Text::Text() {}

string Text::remTabs(string line)
{
  int tab = line.find("\t");
  if(tab == (short)line.npos)
    return line;
  else
    return remTabs(line.replace(tab, 1, "    "));
}

void Text::insertLine(string line, int n)
{
  line = remTabs(line);                   // Conversion (happens every time)
  lines.insert(lines.begin()+n, line);
}

void Text::appendLine(string line)
{
  line = remTabs(line);
  lines.push_back(line);
}

void Text::removeLine(int n)
{
  lines.erase(lines.begin()+n);
}
