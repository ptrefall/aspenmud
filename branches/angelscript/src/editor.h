/*
 *Basic editor object
 */
#ifndef EDITOR_H
#define EDITOR_H
#include <vector>
#include <string>
#include "mud.h"
#include "conf.h"
#include "player.h"
#include "socket.h"
#include "event.h"

class Editor
{
protected:
  Player* _mobile;
  std::vector <std::string> _lines;
  int _cursor;
  BOOL _dirty;
  void* _arg;
  struct in_data *_handler;
public:
  EventManager events;
  Editor(void);
  virtual    ~Editor(void);
  virtual BOOL Load(void);
  virtual void Save(void);
  virtual void Abort(void);
  virtual void Quit(void);
  virtual void List(BOOL lnum);
  virtual void Add(const std::string &line, BOOL quiet = false);
  virtual void Insert(int index);
  virtual void Delete(void);
  virtual void Delete(int index);
  virtual void Delete(int first, int second);
  virtual BOOL EnterEditor(Player* mobile);
  virtual void LeaveEditor(void);
  void SetArg(void* arg);
  void* GetArg() const;
  std::vector<std::string>* GetLines();
};

class EditorInput:public InputHandle
{
public:
  void Input(void* arg, const std::string &input);
};
#endif