section .text
  global _start
                                   
_start:
  push 0x21646c72
  mov rcx, 0x6f77206f6c6c6548
  push rcx
  mov rsi, rsp
  mov rdi, 1
  mov rax, 1
  mov rdx, 13
  syscall