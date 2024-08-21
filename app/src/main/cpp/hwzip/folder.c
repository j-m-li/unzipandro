/*

          21 August MMXXIV PUBLIC DOMAIN by JML

     The authors and contributors disclaim copyright, patents
           and all related rights to this software.

 Anyone is free to copy, modify, publish, use, compile, sell, or
 distribute this software, either in source code form or as a
 compiled binary, for any purpose, commercial or non-commercial,
 and by any means.

 The authors waive all rights to patents, both currently owned
 by the authors or acquired in the future, that are necessarily
 infringed by this software, relating to make, have made, repair,
 use, sell, import, transfer, distribute or configure hardware
 or software in finished or intermediate form, whether by run,
 manufacture, assembly, testing, compiling, processing, loading
 or applying this software or otherwise.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT OF ANY PATENT, COPYRIGHT, TRADE SECRET OR OTHER
 PROPRIETARY RIGHT.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR
 ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include "folder.h"
#include <dirent.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

struct _folder {
    char fldrname[FILENAME_MAX + 4];
    size_t namelen;
    DIR *dfd;
};


FOLDER *openfldr(const char *fldrname)
{
    FOLDER *ffd;

    if (!fldrname) {
	return NULL;
    }
    ffd = malloc(sizeof(*ffd));
    if (!ffd) return NULL;
    ffd->dfd = opendir(fldrname);
    if (!ffd->dfd) {
	free(ffd);
	return NULL;
    }
    ffd->namelen = strlen(fldrname);
    memcpy(ffd->fldrname, fldrname, ffd->namelen);
    ffd->fldrname[ffd->namelen] = '/';
    ffd->namelen++;
    ffd->fldrname[ffd->namelen] = '\0';
    return ffd;
}

char *readfldr(FOLDER *ffd) {
    struct dirent *d;
    size_t l;
    struct stat sb;
    do {
	d = readdir(ffd->dfd);
    } while (d && (!strcmp(d->d_name, ".")
    	|| !strcmp(d->d_name, "..")));
    if (!d) {
	return NULL;
    }
    l = strlen(d->d_name);
    if (l + ffd->namelen >= sizeof(ffd->fldrname) - 2) {
	return NULL;
    }
    memcpy(ffd->fldrname + ffd->namelen, d->d_name, l);
    ffd->fldrname[ffd->namelen + l] = '\0';

    if (stat(ffd->fldrname, &sb) == -1) {
	return NULL;
    }
    if ((sb.st_mode & S_IFMT) == S_IFDIR) {
	ffd->fldrname[ffd->namelen + l] = '/';
	ffd->fldrname[ffd->namelen + l + 1] = '\0';
    }
    return ffd->fldrname + ffd->namelen;
}

int closefldr(FOLDER *ffd)
{
   int r;
   r = closedir(ffd->dfd);
   free(ffd);
   return r;
}

int mkfldr(const char *path)
{
    char temp[FILENAME_MAX];
    char *b;
    char c;
    int r;
    snprintf(temp, sizeof(temp), "%s", path);
    b = temp;
    while (*b) {
	b++;
	while (*b && *b != '/' && *b != '\\') {
	    b++;
	}
	c = *b;
	*b = '\0';
	r = mkdir(temp, 438);
	*b = c;
    }
    return r;
}

int rmfldr(const char *path)
{
    return rmdir(path);
}
