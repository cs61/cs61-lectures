00000000004011a0 <_Z4frogPci>:
  4011a0:       f3 0f 1e fa             endbr64 
  4011a4:       53                      push   %rbx
  4011a5:       89 f3                   mov    %esi,%ebx
  4011a7:       e8 b4 fe ff ff          callq  401060 <strlen@plt>
  4011ac:       39 c3                   cmp    %eax,%ebx
  4011ae:       0f 4c c3                cmovl  %ebx,%eax
  4011b1:       5b                      pop    %rbx
  4011b2:       c3                      retq   
  4011b3:       66 66 2e 0f 1f 84 00    data16 nopw %cs:0x0(%rax,%rax,1)
  4011ba:       00 00 00 00 
  4011be:       66 90                   xchg   %ax,%ax

00000000004011c0 <_Z7strangeiiiiPci>:
  4011c0:       f3 0f 1e fa             endbr64 
  4011c4:       01 f7                   add    %esi,%edi
  4011c6:       01 d7                   add    %edx,%edi
  4011c8:       8d 04 0f                lea    (%rdi,%rcx,1),%eax
  4011cb:       44 39 c8                cmp    %r9d,%eax
  4011ce:       7c 10                   jl     4011e0 <_Z7strangeiiiiPci+0x20>
  4011d0:       44 89 ce                mov    %r9d,%esi
  4011d3:       4c 89 c7                mov    %r8,%rdi
  4011d6:       eb c8                   jmp    4011a0 <_Z4frogPci>
  4011d8:       0f 1f 84 00 00 00 00    nopl   0x0(%rax,%rax,1)
  4011df:       00 
  4011e0:       c3                      retq   
  4011e1:       66 66 2e 0f 1f 84 00    data16 nopw %cs:0x0(%rax,%rax,1)
  4011e8:       00 00 00 00 
  4011ec:       0f 1f 40 00             nopl   0x0(%rax)

00000000004011f0 <_Z8nonsenseiiiiPcS_i>:
  4011f0:       f3 0f 1e fa             endbr64 
  4011f4:       55                      push   %rbp
  4011f5:       4c 89 cd                mov    %r9,%rbp
  4011f8:       53                      push   %rbx
  4011f9:       48 83 ec 08             sub    $0x8,%rsp
  4011fd:       8b 5c 24 20             mov    0x20(%rsp),%ebx
  401201:       41 89 d9                mov    %ebx,%r9d
  401204:       e8 b7 ff ff ff          callq  4011c0 <_Z7strangeiiiiPci>
  401209:       39 c3                   cmp    %eax,%ebx
  40120b:       74 0a                   je     401217 <_Z8nonsenseiiiiPcS_i+0x27>
  40120d:       48 89 ef                mov    %rbp,%rdi
  401210:       e8 3b fe ff ff          callq  401050 <puts@plt>
  401215:       31 c0                   xor    %eax,%eax
  401217:       48 83 c4 08             add    $0x8,%rsp
  40121b:       5b                      pop    %rbx
  40121c:       5d                      pop    %rbp
  40121d:       c3                      retq   
  40121e:       66 90                   xchg   %ax,%ax

0000000000401070 <main>:
  401070:       f3 0f 1e fa             endbr64 
  401074:       48 83 ec 10             sub    $0x10,%rsp
  401078:       b9 04 00 00 00          mov    $0x4,%ecx
  40107d:       ba 03 00 00 00          mov    $0x3,%edx
  401082:       be 02 00 00 00          mov    $0x2,%esi
  401087:       6a 08                   pushq  $0x8
  401089:       4c 8d 0d 74 0f 00 00    lea    0xf74(%rip),%r9        # 402004 <_IO_stdin_used+0x4>
  401090:       bf 01 00 00 00          mov    $0x1,%edi
  401095:       4c 8d 05 70 0f 00 00    lea    0xf70(%rip),%r8        # 40200c <_IO_stdin_used+0xc>
  40109c:       e8 4f 01 00 00          callq  4011f0 <_Z8nonsenseiiiiPcS_i>
  4010a1:       31 c0                   xor    %eax,%eax
  4010a3:       48 83 c4 18             add    $0x18,%rsp
  4010a7:       c3                      retq   
  4010a8:       0f 1f 84 00 00 00 00    nopl   0x0(%rax,%rax,1)
  4010af:       00
