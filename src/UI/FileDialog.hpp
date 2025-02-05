#include <SDL3/SDL.h>
#ifndef FILEDIALOG_HPP_ //Include guard
#define FILEDIALOG_HPP_

// Called when a file is opened, queues each file to be opened.
void SDLCALL fileOpenCallback(void* pFilesOpened, const char* const* fileList, int filter);

// Called when a file is saved.
void SDLCALL fileSaveCallback(void* pfileData, const char* const* fileList, int filter);

#endif
