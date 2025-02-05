#define _GNU_SOURCE
#include "gotcha_dl.h"
#include "tool.h"
#include "libc_wrappers.h"
#include "elf_ops.h"
#include <dlfcn.h>

#if defined(__GLIBC__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 34
#    define GOTCHA_DL_SYM_WORKAROUND 1
#endif

#ifndef GOTCHA_DL_SYM_WORKAROUND
void* _dl_sym(void* handle, const char* name, void* where);
#endif

gotcha_wrappee_handle_t orig_dlopen_handle;
gotcha_wrappee_handle_t orig_dlsym_handle;

static int per_binding(hash_key_t key, hash_data_t data, void *opaque KNOWN_UNUSED)
{
   int result;
   struct internal_binding_t *binding = (struct internal_binding_t *) data;

   debug_printf(3, "Trying to re-bind %s from tool %s after dlopen\n",
                binding->user_binding->name, binding->associated_binding_table->tool->tool_name);

   while (binding->next_binding) {
      binding = binding->next_binding;
      debug_printf(3, "Selecting new innermost version of binding %s from tool %s.\n",
                   binding->user_binding->name, binding->associated_binding_table->tool->tool_name);
   }

   result = prepare_symbol(binding);
   if (result == -1) {
      debug_printf(3, "Still could not prepare binding %s after dlopen\n", binding->user_binding->name);
      return 0;
   }

   removefrom_hashtable(&notfound_binding_table, key);
   return 0;
}

static void* dlopen_wrapper(const char* filename, int flags) {
   typeof(&dlopen_wrapper) orig_dlopen = gotcha_get_wrappee(orig_dlopen_handle);
   void *handle;
   debug_printf(1, "User called dlopen(%s, 0x%x)\n", filename, (unsigned int) flags);
   handle = orig_dlopen(filename,flags);

   debug_printf(2, "Searching new dlopened libraries for previously-not-found exports\n");
   foreach_hash_entry(&notfound_binding_table, NULL, per_binding);

   debug_printf(2, "Updating GOT entries for new dlopened libraries\n");
   update_all_library_gots(&function_hash_table);

   return handle;
}

static void* dlsym_wrapper(void* handle, const char* symbol_name){
  typeof(&dlsym_wrapper) orig_dlsym = gotcha_get_wrappee(orig_dlsym_handle);
  struct internal_binding_t *binding;
  int result;
  debug_printf(1, "User called dlsym(%p, %s)\n", handle, symbol_name);

  if (handle == RTLD_DEFAULT || handle == RTLD_NEXT) {
#if defined GOTCHA_DL_SYM_WORKAROUND
  /* Workaround as _dl_sym isn't exported in glibc 2.34+ anymore.
     This probably breaks dlsym() wrapping for RTLD_NEXT: need to
     wait for real fix in Gotcha.
  */
    return orig_dlsym(handle, symbol_name);
#else
    return _dl_sym(handle, symbol_name ,__builtin_return_address(0));
#endif
  }
  result = lookup_hashtable(&function_hash_table, (hash_key_t) symbol_name, (hash_data_t *) &binding);
  if (result == -1)
     return orig_dlsym(handle, symbol_name);
  else
     return binding->user_binding->wrapper_pointer;
}

struct gotcha_binding_t dl_binds[] = {
  {"dlopen", dlopen_wrapper, &orig_dlopen_handle},
  {"dlsym", dlsym_wrapper, &orig_dlsym_handle}
};
void handle_libdl(){
  gotcha_wrap(dl_binds, 2, "gotcha");
}

