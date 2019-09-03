//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Pretty Cool Fuzzy Guesser (PCFG)
//  --Probabilistic Context Free Grammar (PCFG) Password Guessing Program
//
//  Written by Matt Weir
//  Special thanks to the John the Ripper and Hashcat communities where some 
//  of the code was copied from. And thank you whoever is reading this. Be good!
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//

#include "banner_info.h"


void print_banner(struct program_info program_info) {
	
    fprintf(stderr,"TEMP BANNER GOES HERE\n");
    
    fprintf(stderr,"    ____            __  __           ______            __              \n");
    fprintf(stderr,"   / __ \\________  / /_/ /___  __   / ____/___  ____  / /              \n");
    fprintf(stderr,"  / /_/ / ___/ _ \\/ __/ __/ / / /  / /   / __ \\/ __ \\/ /               \n");
    fprintf(stderr," / ____/ /  /  __/ /_/ /_/ /_/ /  / /___/ /_/ / /_/ / /                \n");
    fprintf(stderr,"/_/ __/_/_  \\___/\\__/\\__/\\__, /   \\__________/\\____/_/                 \n");
    fprintf(stderr,"   / ____/_  __________  __/_/_   / ____/_  _____  _____________  _____\n");
    fprintf(stderr,"  / /_  / / / /_  /_  / / / / /  / / __/ / / / _ \\/ ___/ ___/ _ \\/ ___/\n");
    fprintf(stderr," / __/ / /_/ / / /_/ /_/ /_/ /  / /_/ / /_/ /  __(__  |__  )  __/ /    \n");
    fprintf(stderr,"/_/    /__,_/ /___//__/\\__, /   \\____/\\__,_/\\___/____/____/\\___/_/     \n");
    fprintf(stderr,"                         /_/                                           \n");
    fprintf(stderr," ---------------------------> PURE C EDITION!!!                       \n");
    fprintf(stderr,"Version: %s\n", program_info.version);
    
    return;
}