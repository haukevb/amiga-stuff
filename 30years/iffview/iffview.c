/* iffview.c - IFF ILBM viewer application for Amiga OS >= 1.3

   This file is part of amiga30yrs.

   amiga30yrs is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   amiga30yrs is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with amiga30yrs.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <string.h>

#include <exec/libraries.h>
#include <intuition/intuition.h>

#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#ifdef __VBCC__
#include <clib/alib_stdio_protos.h>
#endif

#define WIN_LEFT       10
#define WIN_TOP        10
#define WIN_WIDTH      320
#define WIN_HEIGHT     200
#define WIN_TITLE      "IFF Viewer"
#define WIN_MIN_WIDTH  10
#define WIN_MIN_HEIGHT 10
#define WIN_MAX_WIDTH  200
#define WIN_MAX_HEIGHT 200

#define FILE_MENU_NUM       0
#define NUM_FILE_MENU_ITEMS 2

#define OPEN_MENU_ITEM_NUM  0
#define QUIT_MENU_ITEM_NUM  1

struct NewWindow newwin = {
  WIN_LEFT, WIN_TOP, WIN_WIDTH, WIN_HEIGHT, 0, 1,
  IDCMP_CLOSEWINDOW | IDCMP_MENUPICK,
  WINDOWCLOSE | SMART_REFRESH | ACTIVATE | WINDOWSIZING | WINDOWDRAG | WINDOWDEPTH | NOCAREREFRESH,
  NULL, NULL, WIN_TITLE,
  NULL, NULL,
  WIN_MIN_WIDTH, WIN_MIN_HEIGHT,
  WIN_MAX_WIDTH, WIN_MAX_HEIGHT,
  WBENCHSCREEN
};

struct IntuiText menutext[] = {
  {0, 1, JAM2, 0, 1, NULL, "Open...", NULL},
  {0, 1, JAM2, 0, 1, NULL, "Quit", NULL}
};

struct MenuItem fileMenuItems[] = {
  {&fileMenuItems[1], 0, 0, 0, 0, ITEMTEXT | ITEMENABLED | HIGHCOMP | COMMSEQ, 0,
   &menutext[0], NULL, 'O', NULL, 0},
  {NULL, 0, 0, 0, 0, ITEMTEXT | ITEMENABLED | HIGHCOMP | COMMSEQ, 0,
   &menutext[1], NULL, 'Q', NULL, 0}
};

struct Menu menus[] = {
  {NULL, 20, 0, 0, 0, MENUENABLED | MIDRAWN, "File", &fileMenuItems[0], 0, 0, 0, 0}
};

struct Window *window;

void cleanup()
{
  if (window) {
    ClearMenuStrip(window);
    CloseWindow(window);
  }
}


#define OK_BUTTON_WIDTH  40
#define OK_BUTTON_HEIGHT 24
#define REQ_WIDTH 200
#define REQ_HEIGHT 100
#define TOPAZ_BASELINE 8

void open_file()
{
  struct Requester requester;
  struct IntuiText button_text = {1, 0, JAM2, 0, TOPAZ_BASELINE, NULL, "Ok", NULL};
  WORD button_border_points[] = {
    0, 0, OK_BUTTON_WIDTH, 0, OK_BUTTON_WIDTH, OK_BUTTON_HEIGHT, 0, OK_BUTTON_HEIGHT, 0, 0
  };
  struct Border button_border = {0, 0, 1, 0, JAM1, 5, button_border_points, NULL};

  WORD req_border_points[] = {
    0, 0, REQ_WIDTH, 0, REQ_WIDTH, REQ_HEIGHT, 0, REQ_HEIGHT, 0, 0
  };
  struct Border req_border = {0, 0, 1, 0, JAM1, 5, req_border_points, NULL};
  BOOL result;

  struct Gadget button1 = {
    NULL, 10, 10, OK_BUTTON_WIDTH, OK_BUTTON_HEIGHT,
    GFLG_GADGHCOMP, 0, GTYP_BOOLGADGET | GTYP_REQGADGET,
    &button_border, NULL, &button_text,
    0, NULL, 0, NULL
  };
  InitRequester(&requester);
  requester.LeftEdge = 20;
  requester.TopEdge = 20;
  requester.Width = REQ_WIDTH;
  requester.Height = REQ_HEIGHT;
  requester.Flags = 0;
  requester.BackFill = 0;
  requester.ReqGadget = &button1;
  requester.ReqBorder = &req_border;
  result = Request(&requester, window);
  if (result) puts("Requester could be opened");
  else puts("Requester could not be opened");
}

BOOL handle_menu(UWORD menuNum, UWORD itemNum, UWORD subItemNum)
{
  printf("menu, menu num: %d, item num: %d, sub item num: %d\n",
         (int) menuNum, (int) itemNum, (int) subItemNum);
  if (menuNum == FILE_MENU_NUM && itemNum == QUIT_MENU_ITEM_NUM) {
    /* quit */
    return TRUE;
  }
  if (menuNum == FILE_MENU_NUM && itemNum == OPEN_MENU_ITEM_NUM) {
    open_file();
  }
  return FALSE;
}

void handle_events() {
  BOOL done = FALSE;
  struct IntuiMessage *msg;
  ULONG msgClass;
  UWORD menuCode;

  while (!done) {
    Wait(1 << window->UserPort->mp_SigBit);
    if (msg = (struct IntuiMessage *) GetMsg(window->UserPort)) {
      msgClass = msg->Class;
      switch (msgClass) {
      case IDCMP_CLOSEWINDOW:
        done = TRUE;
        break;
      case IDCMP_MENUPICK:
        menuCode = msg->Code;
        done = handle_menu(MENUNUM(menuCode), ITEMNUM(menuCode), SUBNUM(menuCode));
        break;
      default:
        break;
      }
      ReplyMsg((struct Message *) msg);
    }
  }
}

void setup_menu()
{
  UWORD txWidth, txHeight, txBaseline, txSpacing, itemWidth, itemHeight, numItems;
  struct RastPort *rp = &window->WScreen->RastPort;
  int i;

  txWidth = rp->TxWidth;
  txHeight = rp->TxHeight;
  txBaseline = rp->TxBaseline;
  txSpacing = rp->TxSpacing;
  printf("TxWidth: %d, TxHeight: %d, TxBaseline: %d, TxSpacing: %d\n",
         (int) txWidth, (int) txHeight, (int) txBaseline, (int) txSpacing);

  /* Set file menu bounds */
  menus[0].Width = TextLength(rp, "File", strlen("File")) + txWidth;
  menus[0].Height = txHeight;

  /* Set file menu items bounds */
  /* We actually need to know what the command uses up */
  itemWidth = txWidth * strlen("Open...") + 50;
  itemHeight = txHeight + 2;  /* 2 pixels adjustment */

  numItems = sizeof(fileMenuItems) / sizeof(struct MenuItem);
  printf("# file items: %d\n", (int) numItems);
  for (i = 0; i < numItems; i++) {
    fileMenuItems[i].TopEdge = i * itemHeight;
    fileMenuItems[i].Height = itemHeight;
    fileMenuItems[i].Width = itemWidth;
  }

  SetMenuStrip(window, &menus[0]);
}

int main(int argc, char **argv)
{
  if (window = OpenWindow(&newwin)) {
    setup_menu();
    handle_events();
  }
  cleanup();
  return 1;
}
