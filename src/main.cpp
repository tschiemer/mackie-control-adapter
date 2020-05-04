#include "main.hpp"

#include <iostream>
#include <getopt.h>
#include <csignal>

using namespace RMETotalMixMidiAdapter;


std::string sessionName = "";
AbstractControlSurface * controlSurface = NULL;
RMETotalMix * rmeTotalMix = NULL;

int sigintTicks = 0;
volatile bool Running = false;

void printHelp( void ) {
    printf("Usage:\n");
    printf("\t rme-totalmix-midi-adapter [-h?]\n");
    printf("\nOptions:\n");
    printf("\t -h|-? \t\t\t\t show this help\n");
    printf("\t -l|--list list available control surfaces\n");
    printf("\t -sNAME|--session-name=NAME\n");
    printf("\n");
}

void printControlSurfaces( void ){
    for(int i = 0; i < ControlSurfaceCount; i++){
      std::cout << ControlSurfaceList[i].Key << std::endl;
    }
}

void SignalHandler(int signal)
{
  // if (SIG)
  sigintTicks++;

  if (sigintTicks == 1){
    std::cerr << "Press CTRL-C again to quit." << std::endl;
    return;
  }

  // Attempt to gracefully stop process.
  Running = false;

  // Force quit if necessary
  if (sigintTicks > 2){
    exit(EXIT_FAILURE);
  }
}

void setupSignalHandler(){
  sigintTicks = 0;
  std::signal(SIGINT, SignalHandler);
}

int main(int argc, char * argv[], char * env[]){


  int c;
  int digit_optind = 0;

  ControlSurfaceFactory controlSurfaceFactory = NULL;

  if (argc <= 1){
      printHelp();
      return EXIT_SUCCESS;
  }

  while (1) {
      int this_option_optind = optind ? optind : 1;
      int option_index = 0;

      static struct option long_options[] = {
        {"control-surface", required_argument, 0, 'c'},
        {"session-name", optional_argument, 0, 's'},
        {"list", no_argument, 0, 'l'},
              // {"parse",    no_argument,    0,  'p' },
              // {"generate",   no_argument,    0,  'g' },
              // {"timed", optional_argument,  0, 't'},
              // {"running-status", optional_argument, 0, 'r'},
              // {"prefix", required_argument, 0, 0},
              // {"suffix", required_argument, 0, 0},
              // {"verbose", optional_argument, 0, 'v'},
              // {"exit-on-error", required_argument, 0, 'x'},
              // {"help",    no_argument,    0,  'h' },
              // {"convert", required_argument, 0, 0},
              // {"hex", no_argument, 0, 0},
              // {"nrpn-filter", no_argument, 0, 'n'},
          {0,         0,              0,  0 }
      };

      c = getopt_long(argc, argv, "h?lc:s:",
                      long_options, &option_index);
      if (c == -1)
          break;

      switch (c) {
          // case 0:

          case '?':
          case 'h':
              printHelp();
              return EXIT_SUCCESS;

          case 'l':
            printControlSurfaces();
            return EXIT_SUCCESS;

          case 'c':
            if (optarg != NULL && strlen(optarg) > 0){
              for (int i = 0; i < ControlSurfaceCount; i++){
                if (ControlSurfaceList[i].Key == optarg){
                  controlSurfaceFactory = ControlSurfaceList[i].Factory;
                  std::cout << "Control Surface: " << ControlSurfaceList[i].Key << std::endl;
                }
              }
            }
            break;

          case 's':
            if (optarg != NULL && strlen(optarg) > 0){
              sessionName = optarg;

              std::cout << "Session Name: " << sessionName << std::endl;
            }
            break;


          default:
              printf("?? getopt returned character code %o ??\n", c);
      }
  }

  // Config validation
  if (controlSurfaceFactory == NULL){
    std::cerr << "ERROR No control surface defined!" << std::endl;
    exit(EXIT_FAILURE);
  }



  controlSurface = controlSurfaceFactory(argc - optind, &argv[optind]);

  rmeTotalMix = new RMETotalMix();

  controlSurface->setRMETotalMix(rmeTotalMix);
  rmeTotalMix->setControlSurface(controlSurface);

  rmeTotalMix->start();
  controlSurface->start();

  setupSignalHandler();
  std::cerr << "Press CTRL-C twice for graceful shutdown." << std::endl;

  Running = true;
  while(Running){}

  rmeTotalMix->stop();
  controlSurface->stop();

  delete rmeTotalMix;
  delete controlSurface;

  return 0;
}
