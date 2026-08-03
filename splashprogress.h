#ifndef SPLASHPROGRESS_H
#define SPLASHPROGRESS_H

// Pointeur de fonction global, utilisé pour faire remonter une progression
// réelle (0-100) depuis l'énumération des ports série jusqu'à l'écran de
// démarrage, sans changer l'architecture du programme.
typedef void (*SplashProgressCallback)(int percent);
extern SplashProgressCallback g_splashProgressCallback;

#endif // SPLASHPROGRESS_H
