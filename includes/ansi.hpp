#ifndef ANSI_HPP
# define ANSI_HPP

# include <sstream>

/*_____________FOR USE____________*/
/*
Starting with \033[ then add your style
(for serveral choices separete with ;),
add your color and finish with 'm'

Note : The white color is default,
so there is no need to add it to use it.
*/

//      STYLE
# define NORMAL 0
# define BOLD 1
# define DARK 2
# define ITALIC 3
# define UNDERLINE 4
# define BLINK 5
# define HIGHLIGHT 7
# define TRANSPARENT 8
# define STRIKETHROUGH 9

//      COLOR
# define BLACK 30
# define RED 31
# define GREEN 32
# define YELLOW 33
# define BLUE 34
# define PURPLE 35
# define CYAN 36

std::string	ansi(const short set[], short size);

//------Sous C++17-------

// STYLE
// # define NORMAL "0"
// # define BOLD "1"
// # define DARK "2"
// # define ITALIC "3"
// # define UNDERLINE "4"
// # define BLINK "5"
// # define HIGHLIGHT "7"
// # define TRANSPARENT "8"
// # define STRIKETHROUGH "9"

// // COLOR
// # define BLACK "30"
// # define RED "31"
// # define GREEN "32"
// # define YELLOW "33"
// # define BLUE "34"
// # define PURPLE "35"
// # define CYAN "36"
// # define WHITE "37"
// // LIGHT COLOR
// # define LBLACK "90"
// # define LRED "91"
// # define LGREEN "92"
// # define LYELLOW "93"
// # define LBLUE "94"
// # define LPURPLE "95"
// # define LCYAN "96"
// # define LWHITE "97"
// // BACKGROUND COLOR
// # define BBLACK "40"
// # define BRED "41"
// # define BGREEN "42"
// # define BYELLOW "43"
// # define BBLUE "44"
// # define BPURPLE "45"
// # define BCYAN "46"
// # define BWHITE "47"
// // BACKGROUND LIGHT COLOR
// # define BLBLACK "100"
// # define BLRED "101"
// # define BLGREEN "102"
// # define BLYELLOW "103"
// # define BLBLUE "104"
// # define BLPURPLE "105"
// # define BLCYAN "106"
// # define BLWHITE "107"

// // CURSOR
// # define CSAVE "s"		// Sauvegarder la position actuelle du curseu
// # define CRESTORE "u"	// Restaurer la position du curseur précédemment sauvegardée
// # define CUP "A"		// Déplacer le curseur vers le haut de 1 ligne
// # define CDOWN "B"		// Déplacer le curseur vers le bas de 1 ligne
// # define CRIGHT "C"		// Déplacer le curseur vers la droite de 1 colonne
// # define CLEFT "D"		// Déplacer le curseur vers la gauche de 1 colonne

// // DELETE
// # define DENDSCREEN "0J"	// Effacer depuis le début de l'écran jusqu'à la position du curseur
// # define DSTARTSCREEN "1J"	// Effacer depuis le début de l'écran jusqu'à la position du curseur
// # define DALL "2J"			// Effacer tout l'écran
// # define DENDLINE "0K"		// Effacer depuis la position du curseur jusqu'à la fin de la ligne
// # define DSTARTLINE "1K"	// Effacer depuis le début de la ligne jusqu'à la position du curseur
// # define DLINE "2K"			// Effacer la ligne courante

// template <typename... Strings>
// std::string ansi(Strings... strings);

#endif
