/*
 * Copyright (C) 2008 IsmAvatar <cmagicj@nni.com>
 * Copyright (C) 2010 Alasdair Morrison <tgmg@g-java.com>
 *
 * This file is part of ENIGMA.
 *
 * ENIGMA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ENIGMA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License (COPYING) for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <unistd.h>
#include <stdio.h>
#include "CocoaMain.h"
#include "ObjectiveC.h"

#include "../General/PFwindow.h"
#include "../General/PFfilemanip.h"

#include "Universal_System/CallbackArrays.h"
#include "Universal_System/roomsystem.h"

namespace enigma_user {
  std::string working_directory = "";
}

int main(int argc,char** argv)
{
  // Set the working_directory
  char buffer[1024];
  if (getcwd(buffer, sizeof(buffer)) != NULL)
     fprintf(stdout, "Current working dir: %s\n", buffer);
  else
     perror("getcwd() error");
  enigma_user::working_directory = string( buffer );
  
	enigma::parameters=new char* [argc];
	for (int i=0; i<argc; i++)
		enigma::parameters[i]=argv[i];

	return mainO(argc, argv);
}

namespace enigma_user {

  void sleep(int ms) {
    if (ms > 1000) ::sleep(ms/1000);
    usleep((ms % 1000) *1000);
  };
  
  void game_end(int ret) {
    //audiosystem_cleanup();
    exit(ret);
  }

  void action_end_game()
  {
    game_end();
  }

}
