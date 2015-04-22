/*  

    Copyright (c) 2015 Oliver Lau <ola@ct.de>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/


#include "stdafx.h"

#if defined(WIN32)
#include <Windows.h>
#elif defined(LINUX_AMD64)
#include <gtk/gtk.h>
#endif


int main(int argc, char *argv[])
{
  Impact::Game *breakout = new Impact::Game;
  if (breakout == nullptr)
    return EXIT_FAILURE;
#if defined(LINUX_AMD64)   
  gtk_init(&argc, &argv);
#endif
  if (argc == 2) {
#if defined(WIN32) && defined(CT_VERSION_INTERNAL)
    char szPath[MAX_PATH];
    char *res = _fullpath(szPath, argv[1], MAX_PATH);
    if (res != NULL) {
      DWORD dwAttrib = GetFileAttributes(szPath);
      if (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))
        breakout->setLevelZip(szPath);
    }
#else
    UNUSED(argv);
#endif
  }
  breakout->loop();
  delete breakout;
  return EXIT_SUCCESS;
}
