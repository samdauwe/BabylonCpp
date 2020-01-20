#ifndef BABYLONCPP_SUBPROCESS_CPP_WRAPPER_H
#define BABYLONCPP_SUBPROCESS_CPP_WRAPPER_H

#include "subprocess.h"
#include <string>
#include <vector>

inline subprocess_weak int subprocess_create_cpp(const std::vector<std::string> command_line_vector,
                                      int options,
                                      struct subprocess_s *const out_process)
{
  const char ** command_line_c;
  command_line_c = (const char **) malloc((command_line_vector.size() + 1) * sizeof(char *));
  for (std::size_t i = 0; i < command_line_vector.size(); ++i)
    command_line_c[i] = command_line_vector[i].c_str();
  command_line_c[command_line_vector.size()] = NULL;

   auto result = subprocess_create(command_line_c, options, out_process);
   free(command_line_c);

  return result;
}


#endif // BABYLONCPP_SUBPROCESS_CPP_WRAPPER_H
