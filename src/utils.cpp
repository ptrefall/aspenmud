#include <string>
#include <sstream>
#include <list>
#include <map>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sys/stat.h>
#include <dirent.h>
#include "utils.h"
#include "editor.h"

inline BOOL IsAlpha(const char* const character)
{
  int asc=(int)*character;
  if (((asc>=65)&&(asc<=90))||((asc>=97)&&(asc<=122)))
    {
      return true;
    }
  return false;
}
BOOL FileExists(const std::string &name)
{
  struct stat info;
  int res=stat(name.c_str(),&info);
  if (res==0)
    {
      return true;
    }
  return false;
}

UINT GetFileSize(int fd)
{
  struct stat info;
  int res=fstat(fd, &info);
  if (res)
    {
      return -1;
    }
  else
    {
      return info.st_size;
    }
}
UINT GetFileSize(const char* path)
{
  struct stat info;
  int res = stat(path, &info);
  if (res)
    {
      return -1;
    }
  else
    {
      return info.st_size;
    }
}
UINT GetFileSize(const std::string &path)
{
  return GetFileSize(path.c_str());
}


void Tokenize(const std::string &str, std::vector<std::string> &tokens, const std::string &del)
{
  std::string::size_type lpos=str.find_first_not_of(del,0);
  std::string::size_type pos=str.find_first_of(del,lpos);
  if (pos == std::string::npos)
    {
      tokens.push_back(str);
      return;
    }
//our loop for tokenizing:
  while ((std::string::npos!=pos)||(std::string::npos!=lpos))
    {
      tokens.push_back(str.substr(lpos,(pos-lpos)));
      lpos=str.find_first_not_of(del,pos);
      pos=str.find_first_of(del,lpos);
    }
}

void SplitToVector(const std::string &line, std::vector<std::string>* output, int cols)
{
  if (line.length() <= (unsigned int)cols)
    {
      output->push_back(line);
      return;
    }

  size_t start, length, cur;
  length = line.length();
  cur = start = 0;

  while (1)
    {
      cur = start+cols; //move the pointer to the maximum length from start.
      if ((int)(length - start) <= cols) //the rest of the line is less than cols, so we don't need to split.
        {
          output->push_back(line.substr(start, length));
          return;
        }

      for (;; --cur) //we decrement cur until we find a space:
        {
          if (line[cur] == ' ') //we found a space in our search backwards.
            {
              output->push_back(line.substr(start, cur-start)); //pushes the substr between start and cur.
              start = cur+1; //moves start to cur+1, which is after the space.
              break;
            }
        }
    }
}

BOOL IsValidUserName(const std::string &input)
{
  const char* ptr=input.c_str();
  if ((input.length()<4)||(input.length()>11))
    {
      return false;
    }
  while (*ptr)
    {
      if (!IsAlpha(ptr))
        {
          return false;
        }
      ptr++;
    }
  return true;
}

BOOL IsValidPassword(const std::string &input)
{
  std::string::const_iterator it, itEnd;

  if ((input.length()<5)||(input.length()>20))
    {
      return false;
    }

  itEnd = input.end();
  for (it = input.begin(); it != itEnd; ++it)
    {
      if (!isgraph((*it)))
        {
          return false;
        }
    }

  return true;
}

BOOL PlayerExists(const std::string &name)
{
  return FileExists(PLAYER_DIR+name);
}

BOOL IsFirstUser()
{
  int count = 0;
  std::string path=(PLAYER_DIR);
  DIR* search=opendir(path.c_str());
  struct dirent* dir = NULL;

  if (!search)
    {
      return false;
    }

  while (1)
    {
      dir=readdir(search);
      if (!dir)
        {
          break;
        }
      if (dir->d_name[0] == '.')
        {
          continue;
        }
      else
        {
          count = 1;
          break;
        }
    }

  return (count==1?false:true);
}

void Lower(std::string &str)
{
  int i;
  int length=str.length();
  for (i=0; i<length; i++)
    str[i]=tolower(str[i]);
}

std::string Capitalize(const std::string &str)
{
  std::string ret=str;
  if (str.length()>=1)
    {
      ret[0]=toupper(str[0]);
    }
  return ret;
}

std::string Repete(const std::string &filler,const int count)
{
  std::string ret;
  int i;
  for (i=0; i<count; i++)
    {
      ret+=filler;
    }
  return ret;
}

std::string Center(const std::string &str,const int width)
{
  std::string temp;
  int left=0;
  int right=0;
  int length=(int)str.length();
  int result=0;

//the length was greater than the width, no centering possible:
  if (length >= width)
    {
      return str;
    }
//calculate how many "filler"chars will be needed
//If there is a remainder of one, it will add the extra one space to the left side
  result=length%2;
  right = (length/2);
  left = length/2;
//remainder of 1.
  if (result)
    {
      left++;
    }

  temp=Repete(" ",left);
  temp += str;
  temp += Repete(" ", right);

  return temp;
}

std::string Explode(std::vector <std::string> &parts, const std::string &del)
{
  std::vector<std::string>::iterator it, itEnd;
  std::stringstream st;

  itEnd = parts.end();
  it = parts.begin();
//we need to handle the first element before the loop so we don't have a space in the beginning.
  if (it != itEnd)
    {
      st << (*it);
      ++it;
    }
  for (; it != itEnd; ++it)
    {
      st << del << (*it);
    }

  return st.str();
}
std::string StripWhitespace(const std::string &str)
{
  if (str.find_first_of(" \r\n")==std::string::npos)
    {
      return str;
    }
  else
    {
      return (str.substr(str.find_first_of(" \r\n")));
    }
}
void NumberToString(char* buffer, int number)
{
  sprintf(buffer, "%d", number);
}

std::string Columnize(std::vector<std::string> *data, int cols, std::vector<std::string>* headers, int width)
{
  int cwidth = (width/cols);
  int count = (int)data->size();
  std::vector<std::string>::iterator it, itEnd;
  int i = 0;
  int j=0;
  std::stringstream st;

  if (!count)
    {
      return "";
    }
  if (headers && (int)headers->size() != cols)
    {
      return "";
    }
  if (headers)
    {
      itEnd = headers->end();
      for (it = headers->begin(); it != headers->end(); ++it)
        {
          int res = cwidth - (*it).length();
          st << (*it);
          for (j=0; j < res; j++)
            {
              st << " ";
            }
        }
    }
  st << "\n" << Repete("-", width) << "\n";

  itEnd = data->end();
  for (it = data->begin(), i = 1; it != itEnd; ++it, ++i)
    {
      if ((int)(*it).length() <= cwidth)
        {
          int res=(cwidth-(*it).length());
          st << (*it);
          for (j = 0; j < res; j++)
            {
              st << " ";
            }
        }
      if (i == cols)
        {
          st << "\n";
          i=0;
        }
    }
  return (st.str());
}

FLAG BitSet(FLAG flag,int pos)
{
  return flag|=pos;
}
FLAG BitClear(FLAG flag,int pos)
{
  return flag&=~pos;
}
BOOL BitIsSet(FLAG flag,int pos)
{
  return (flag & pos) == pos;
}

std::string EnglishList(std::vector<std::string> *in)
{
  std::vector <std::string>::iterator it;
  std::string str="";

  if (!in->size())
    {
      return "";
    }
  if (in->size()==1)
    {
      return (*in->begin());
    }

  for (it=in->begin(); it!=in->end(); it++)
    {
      if ((*it)==in->back())
        {
          str+="and "+(*it);
          break;
        }
      str+=(*it)+", ";
    }

  return str;
}

int Roll(int n,int s)
{
  int result=0;
  int i;

  if (n<=0)
    {
      n=1;
    }
  if (s<=0)
    {
      s=1;
    }

  for (i=0; i<n; i++)
    {
      result+=(rand()%(s+1));
    }

  return result;
}

std::string SwapExit(const std::string &exit)
{
  if ((exit=="n")||(exit=="north"))
    {
      return "south";
    }
  else if ((exit=="s")||(exit=="south"))
    {
      return "north";
    }
  else if ((exit=="e")||(exit=="east"))
    {
      return "west";
    }
  else if ((exit=="w")||(exit=="west"))
    {
      return "east";
    }
  else if ((exit=="ne")||(exit=="northeast"))
    {
      return "southwest";
    }
  else if ((exit=="nw")||(exit=="northwest"))
    {
      return "southeast";
    }
  else if ((exit=="se")||(exit=="southeast"))
    {
      return "northwest";
    }
  else if ((exit=="sw")||(exit=="southwest"))
    {
      return "northeast";
    }
  else if ((exit=="u")||(exit=="up"))
    {
      return "down";
    }
  else if ((exit=="d")||(exit=="down"))
    {
      return "up";
    }
  else
    {
      return "";
    }
}

BOOL IsValidExit(const std::string &name)
{
  if ((name == "north" || name == "n") ||
      (name == "south" || name == "s") ||
      (name == "east" || name == "e") ||
      (name == "west" || name == "w") ||
      (name == "northeast" || name == "ne") ||
      (name == "northwest" || name == "nw") ||
      (name == "southeast" || name == "se") ||
      (name == "southwest" || name == "sw"))
    {
      return true;
    }
  else
    {
      return false;
    }
}
ExitDirection GetDirectionByName(const std::string &name)
{
  if (name == "north" || name == "n")
    return north;
  else if (name == "south" || name == "s")
    return south;
  else if (name == "east" || name == "e")
    return east;
  else if (name == "west" || name == "w")
    return west;
  else if (name == "northeast" || name == "ne")
    return northeast;
  else if (name == "northwest" || name == "nw")
    return northwest;
  else if (name == "southwest" || name == "sw")
    return southwest;
  else if (name == "southeast" || name == "se")
    return southeast;
  else
    return nowhere;
}

void TimeInfo::Initialize(void)
{
  hour=0;
  minute=0;
  second=0;
}

TimeInfo::TimeInfo(time_t data)
{
  Initialize();
  Calculate(data);
}
TimeInfo::TimeInfo(UINT data)
{
  Initialize();
  Calculate((time_t)data);
}
TimeInfo::TimeInfo(void)
{
  Initialize();
}

void TimeInfo::Calculate(time_t data)
{
  UINT count=data;
  hour=count/3600;
  count%=3600;
  minute=count/60;
  count%=60;
  second=count;
}

std::string TimeInfo::ToString(void)
{
  std::stringstream st;
  st << hour << ":" << minute << ":" << second;
  return st.str();
}

point::point()
{
  x = y = z = 0;
}
point::point(int X, int Y, int Z)
{
  x = X;
  y = Y;
  z = Z;
}

BOOL point::operator ==(const point &p)
{
  return (p.x == x && p.y == y && p.z == z);
}

point& point::operator =(point& p)
{
  x = p.x;
  y = p.y;
  z = p.z;
  return *this;
}
