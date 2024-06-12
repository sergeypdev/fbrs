.type fbrs_save_context, @function
.global fbrs_save_context
fbrs_save_context:
  # Save the return address and stack pointer.
  movq (%rsp), %r8
  movq %r8, 8*0(%rdi) # RIP
  leaq 8(%rsp), %r8
  movq %r8, 8*1(%rdi) # RSP

  # Save preserved registers.
  movq %rbx, 8*2(%rdi)
  movq %rbp, 8*3(%rdi)
  movq %r12, 8*4(%rdi)
  movq %r13, 8*5(%rdi)
  movq %r14, 8*6(%rdi)
  movq %r15, 8*7(%rdi)

  # Return.
  xorl %eax, %eax
  ret


.type fbrs_load_context, @function
.global fbrs_load_context
fbrs_load_context:
  # Load RIP
  movq 8*0(%rdi), %r8

  # Load stack pointer
  movq 8*1(%rdi), %rsp

  movq 8*2(%rdi), %rbx
  movq 8*3(%rdi), %rbp
  movq 8*4(%rdi), %r12
  movq 8*5(%rdi), %r13
  movq 8*6(%rdi), %r14
  movq 8*7(%rdi), %r15

  # Push RIP on stack for RET
  pushq %r8

  xorl %eax, %eax
  ret

.type fbrs_switch_context, @function
.global fbrs_switch_context
fbrs_switch_context:
  # Save the return address and stack pointer.
  movq (%rsp), %r8
  movq %r8, 8*0(%rdi) # RIP
  leaq 8(%rsp), %r8
  movq %r8, 8*1(%rdi) # RSP

  # Save preserved registers.
  movq %rbx, 8*2(%rdi)
  movq %rbp, 8*3(%rdi)
  movq %r12, 8*4(%rdi)
  movq %r13, 8*5(%rdi)
  movq %r14, 8*6(%rdi)
  movq %r15, 8*7(%rdi)

  # Load RIP
  movq 8*0(%rsi), %r8

  # Load stack pointer
  movq 8*1(%rsi), %rsp

  movq 8*2(%rsi), %rbx
  movq 8*3(%rsi), %rbp
  movq 8*4(%rsi), %r12
  movq 8*5(%rsi), %r13
  movq 8*6(%rsi), %r14
  movq 8*7(%rsi), %r15

  # Push RIP on stack for RET
  pushq %r8

  xorl %eax, %eax
  ret
