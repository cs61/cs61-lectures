set arch i386:x86-64

init-if-undefined $fun_continue = 0
init-if-undefined $fun_file = ""
init-if-undefined $fun_file_set = 0
init-if-undefined $fun_argc = 0
init-if-undefined $fun_arg0 = ""
init-if-undefined $fun_arg1 = ""
init-if-undefined $fun_arg2 = ""
init-if-undefined $fun_arg3 = ""

define fun-file
  if $argc != 1
    expected_exactly_one_argument
  else
    file $arg0
    set $fun_file = $arg0
    set $fun_file_set = 1
    p $fun_file
  end
end

document fun-file
  Set the fun-file to run.
end

alias -a ff = fun-file

define run-fun
  if $fun_file_set == 0
    please_run_fun_file_first
  else
    if $argc > 0
      set $fun_argc = $argc
      set $fun_arg0 = $arg0
    end
    if $argc > 1
      set $fun_arg1 = $arg1
    end
    if $argc > 2
      set $fun_arg2 = $arg2
    end
    if $argc > 3
      set $fun_arg3 = $arg3
    end
    if $argc > 4
      too_many_arguments
    end
    if $_thread != 0
      kill
    end
    shell killall qemu-x86_64 >/dev/null 2>&1
    if $fun_argc == 0
      eval "shell qemu-x86_64 -g 12948 \"%s\" & sleep 0.2", $fun_file
    end
    if $fun_argc == 1
      eval "shell qemu-x86_64 -g 12948 \"%s\" \"%s\" & sleep 0.2", $fun_file, $fun_arg0
    end
    if $fun_argc == 2
      eval "shell qemu-x86_64 -g 12948 \"%s\" \"%s\" \"%s\" & sleep 0.2", $fun_file, $fun_arg0, $fun_arg1
    end
    if $fun_argc == 3
      eval "shell qemu-x86_64 -g 12948 \"%s\" \"%s\" \"%s\" \"%s\" & sleep 0.2", $fun_file, $fun_arg0, $fun_arg1, $fun_arg2
    end
    if $fun_argc == 4
      eval "shell qemu-x86_64 -g 12948 \"%s\" \"%s\" \"%s\" \"%s\" \"%s\" & sleep 0.2", $fun_file, $fun_arg0, $fun_arg1, $fun_arg2, $fun_arg3
    end
    target remote localhost:12948
    if $fun_continue != 0
      continue
    end
  end
end

document run-fun
  Restart the current fun-file.
end

alias -a rf = run-fun

ff "debugme"
