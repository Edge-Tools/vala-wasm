#include <pthread.h>
#include <spawn.h>
#include <sys/types.h>
#include <errno.h>
#include <stddef.h>

int pthread_getname_np (pthread_t thread, char *name, size_t len)
{
  (void) thread;
  if (name != NULL && len > 0)
    name[0] = '\0';
  return 0;
}

int posix_spawnp (pid_t *pid, const char *file,
                  const posix_spawn_file_actions_t *file_actions,
                  const posix_spawnattr_t *attrp,
                  char *const argv[], char *const envp[])
{
  (void) pid; (void) file; (void) file_actions;
  (void) attrp; (void) argv; (void) envp;
  return ENOSYS;
}

int res_query (const char *dname, int class_, int type,
               unsigned char *answer, int anslen)
{
  (void) dname; (void) class_; (void) type; (void) answer; (void) anslen;
  return -1;
}

int __wrap_g_spawn_command_line_sync (const char *command_line,
                                      char **standard_output,
                                      char **standard_error,
                                      int *wait_status,
                                      void *error)
{
  (void) command_line; (void) error;
  if (standard_output) *standard_output = 0;
  if (standard_error)  *standard_error = 0;
  if (wait_status)     *wait_status = 1;   return 0;                              }
