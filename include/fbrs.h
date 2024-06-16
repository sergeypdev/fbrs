#ifndef FBRS_H
#define FBRS_H

// CPU detection
#if defined(__x86_64__) || defined(_M_X64)
#define CPU_X64
#elif defined(__aarch64__) || defined(_M_ARM64)
#define CPU_AARCH64
#endif

#ifdef _MSC_VER
#define ABI_WIN64
#else
#define ABI_SYSV
#endif

typedef unsigned long fbrs_ptr_t;

struct fbrs_context_t {
  // put registers here
#ifdef CPU_X64

#ifdef ABI_SYSV
  void *ip, *sp;
  void *rbx, *rbp, *r12, *r13, *r14, *r15;
#endif // ABI_SYSV

#ifdef ABI_WIN64
  void *ip, *sp;
  void *rbx, *rbp, *rsi, *rdi, *r12, *r13, *r14, *r15;
  __m128 xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
#endif // ABI_WIN64

#endif // CPU_X64

#ifdef CPU_AARCH64

  void *ip, *sp; // link register and stack pointer
  void *r19, *r20, *r21, *r22, *r23, *r24, *r25, *r26, *r27, *r28, *r29;
  double v8, v9, v10, v11, v12, v13, v14, v15; // lower 64 bits of v8-v15

#endif // CPU_AARCH64
};

// This function will give you a stack pointer that is correctly aligned
// and can be used for a new fiber.
void *fbrs_get_stack_pointer(void *stack_memory, fbrs_ptr_t stack_size);
void fbrs_save_context(struct fbrs_context_t *context);
void fbrs_load_context(struct fbrs_context_t *context);
void fbrs_switch_context(struct fbrs_context_t *old_context,
                         struct fbrs_context_t *new_context);

struct fbrs_context_t *fbrs_current_context();

#endif // FBRS_H

#define FBRS_IMPLEMENTATION

#ifdef FBRS_IMPLEMENTATION

#ifdef _MSC_VER
#define DECL_SECTION_TEXT                                                      \
  __pragma(section(".text")) __declspec(allocate(".text"))

#define THREAD_LOCAL __declspec(thread)
#else
#define DECL_SECTION_TEXT __attribute__((section(".text#")))

#define THREAD_LOCAL __thread
#endif

#ifdef CPU_X64

#ifdef ABI_SYSV

DECL_SECTION_TEXT unsigned char fbrs_save_context_code[] = {
    0x4c, 0x8b, 0x04, 0x24,       // mov    (%rsp), %r8
    0x4c, 0x89, 0x07,             // mov       %r8, (%rdi)
    0x4c, 0x8d, 0x44, 0x24, 0x08, // lea 0x8(%rsp), %r8
    0x4c, 0x89, 0x47, 0x08,       // mov       %r8, 0x8(%rdi)
    0x48, 0x89, 0x5f, 0x10,       // mov      %rbx, 0x10(%rdi)
    0x48, 0x89, 0x6f, 0x18,       // mov      %rbp, 0x18(%rdi)
    0x4c, 0x89, 0x67, 0x20,       // mov      %r12, 0x20(%rdi)
    0x4c, 0x89, 0x6f, 0x28,       // mov      %r13, 0x28(%rdi)
    0x4c, 0x89, 0x77, 0x30,       // mov      %r14, 0x30(%rdi)
    0x4c, 0x89, 0x7f, 0x38,       // mov      %r15, 0x38(%rdi)
    0x31, 0xc0,                   // xor      %eax, %eax
    0xc3,                         // ret
};

DECL_SECTION_TEXT unsigned char fbrs_load_context_code[] = {
    0x4c, 0x8b, 0x07,       // mov       (%rdi), %r8
    0x48, 0x8b, 0x67, 0x08, // mov    0x8(%rdi), %rsp
    0x48, 0x8b, 0x5f, 0x10, // mov   0x10(%rdi), %rbx
    0x48, 0x8b, 0x6f, 0x18, // mov   0x18(%rdi), %rbp
    0x4c, 0x8b, 0x67, 0x20, // mov   0x20(%rdi), %r12
    0x4c, 0x8b, 0x6f, 0x28, // mov   0x28(%rdi), %r13
    0x4c, 0x8b, 0x77, 0x30, // mov   0x30(%rdi), %r14
    0x4c, 0x8b, 0x7f, 0x38, // mov   0x38(%rdi), %r15
    0x41, 0x50,             // push         %r8
    0x31, 0xc0,             // xor         %eax, %eax
    0xc3,                   // ret
};

DECL_SECTION_TEXT unsigned char fbrs_switch_context_code[] = {
    0x4c, 0x8b, 0x04, 0x24,       // mov    (%rsp),%r8
    0x4c, 0x89, 0x07,             // mov    %r8,(%rdi)
    0x4c, 0x8d, 0x44, 0x24, 0x08, // lea    0x8(%rsp),%r8
    0x4c, 0x89, 0x47, 0x08,       // mov    %r8,0x8(%rdi)
    0x48, 0x89, 0x5f, 0x10,       // mov    %rbx,0x10(%rdi)
    0x48, 0x89, 0x6f, 0x18,       // mov    %rbp,0x18(%rdi)
    0x4c, 0x89, 0x67, 0x20,       // mov    %r12,0x20(%rdi)
    0x4c, 0x89, 0x6f, 0x28,       // mov    %r13,0x28(%rdi)
    0x4c, 0x89, 0x77, 0x30,       // mov    %r14,0x30(%rdi)
    0x4c, 0x89, 0x7f, 0x38,       // mov    %r15,0x38(%rdi)
    0x4c, 0x8b, 0x06,             // mov    (%rsi),%r8
    0x48, 0x8b, 0x66, 0x08,       // mov    0x8(%rsi),%rsp
    0x48, 0x8b, 0x5e, 0x10,       // mov    0x10(%rsi),%rbx
    0x48, 0x8b, 0x6e, 0x18,       // mov    0x18(%rsi),%rbp
    0x4c, 0x8b, 0x66, 0x20,       // mov    0x20(%rsi),%r12
    0x4c, 0x8b, 0x6e, 0x28,       // mov    0x28(%rsi),%r13
    0x4c, 0x8b, 0x76, 0x30,       // mov    0x30(%rsi),%r14
    0x4c, 0x8b, 0x7e, 0x38,       // mov    0x38(%rsi),%r15
    0x41, 0x50,                   // push   %r8
    0x31, 0xc0,                   // xor    %eax,%eax
    0xc3,                         // ret
};
#endif // ABI_SYSV

#ifdef ABI_WIN64

DECL_SECTION_TEXT unsigned char fbrs_save_context_code[] = {
    0x4c, 0x8b, 0x04, 0x24,       // mov    (%rsp),%r8
    0x4c, 0x89, 0x01,             // mov    %r8,(%rcx)
    0x4c, 0x8d, 0x44, 0x24, 0x08, // lea    0x8(%rsp),%r8
    0x4c, 0x89, 0x41, 0x08,       // mov    %r8,0x8(%rcx)
    0x48, 0x89, 0x59, 0x10,       // mov    %rbx,0x10(%rcx)
    0x48, 0x89, 0x69, 0x18,       // mov    %rbp,0x18(%rcx)
    0x48, 0x89, 0x71, 0x20,       // mov    %rsi,0x20(%rcx)
    0x48, 0x89, 0x79, 0x28,       // mov    %rdi,0x28(%rcx)
    0x4c, 0x89, 0x61, 0x30,       // mov    %r12,0x30(%rcx)
    0x4c, 0x89, 0x69, 0x38,       // mov    %r13,0x38(%rcx)
    0x4c, 0x89, 0x71, 0x40,       // mov    %r14,0x40(%rcx)
    0x4c, 0x89, 0x79, 0x48,       // mov    %r15,0x48(%rcx)
    0x0f, 0x11, 0x71, 0x50,       // movups %xmm6,0x50(%rcx)
    0x0f, 0x11, 0x79, 0x60,       // movups %xmm7,0x60(%rcx)
    0x44, 0x0f, 0x11, 0x41, 0x70, // movups %xmm8,0x70(%rcx)
    0x44, 0x0f, 0x11, 0x89, 0x80, //
    0x00, 0x00, 0x00,             // movups %xmm9,0x80(%rcx)
    0x44, 0x0f, 0x11, 0x91, 0x90, //
    0x00, 0x00, 0x00,             // movups %xmm10,0x90(%rcx)
    0x44, 0x0f, 0x11, 0x99, 0xa0, //
    0x00, 0x00, 0x00,             // movups %xmm11,0xa0(%rcx)
    0x44, 0x0f, 0x11, 0xa1, 0xb0, //
    0x00, 0x00, 0x00,             // movups %xmm12,0xb0(%rcx)
    0x44, 0x0f, 0x11, 0xa9, 0xc0, //
    0x00, 0x00, 0x00,             // movups %xmm13,0xc0(%rcx)
    0x44, 0x0f, 0x11, 0xb1, 0xd0, //
    0x00, 0x00, 0x00,             // movups %xmm14,0xd0(%rcx)
    0x44, 0x0f, 0x11, 0xb9, 0xe0, //
    0x00, 0x00, 0x00,             // movups %xmm15,0xe0(%rcx)
    0x31, 0xc0,                   // xor    %eax,%eax
    0xc3,                         // ret
};

DECL_SECTION_TEXT unsigned char fbrs_load_context_code[] = {
    0x4c, 0x8b, 0x01,             // mov    (%rcx),%r8
    0x48, 0x8b, 0x61, 0x08,       // mov    0x8(%rcx),%rsp
    0x48, 0x8b, 0x59, 0x10,       // mov    0x10(%rcx),%rbx
    0x48, 0x8b, 0x69, 0x18,       // mov    0x18(%rcx),%rbp
    0x48, 0x8b, 0x71, 0x20,       // mov    0x20(%rcx),%rsi
    0x48, 0x8b, 0x79, 0x28,       // mov    0x28(%rcx),%rdi
    0x4c, 0x8b, 0x61, 0x30,       // mov    0x30(%rcx),%r12
    0x4c, 0x8b, 0x69, 0x38,       // mov    0x38(%rcx),%r13
    0x4c, 0x8b, 0x71, 0x40,       // mov    0x40(%rcx),%r14
    0x4c, 0x8b, 0x79, 0x48,       // mov    0x48(%rcx),%r15
    0x0f, 0x10, 0x71, 0x50,       // movups 0x50(%rcx),%xmm6
    0x0f, 0x10, 0x79, 0x60,       // movups 0x60(%rcx),%xmm7
    0x44, 0x0f, 0x10, 0x41, 0x70, // movups 0x70(%rcx),%xmm8
    0x44, 0x0f, 0x10, 0x89, 0x80, //
    0x00, 0x00, 0x00,             // movups 0x80(%rcx),%xmm9
    0x44, 0x0f, 0x10, 0x91, 0x90, //
    0x00, 0x00, 0x00,             // movups 0x90(%rcx),%xmm10
    0x44, 0x0f, 0x10, 0x99, 0xa0, //
    0x00, 0x00, 0x00,             // movups 0xa0(%rcx),%xmm11
    0x44, 0x0f, 0x10, 0xa1, 0xb0, //
    0x00, 0x00, 0x00,             // movups 0xb0(%rcx),%xmm12
    0x44, 0x0f, 0x10, 0xa9, 0xc0, //
    0x00, 0x00, 0x00,             // movups 0xc0(%rcx),%xmm13
    0x44, 0x0f, 0x10, 0xb1, 0xd0, //
    0x00, 0x00, 0x00,             // movups 0xd0(%rcx),%xmm14
    0x44, 0x0f, 0x10, 0xb9, 0xe0, //
    0x00, 0x00, 0x00,             // movups 0xe0(%rcx),%xmm15
    0x41, 0x50,                   // push   %r8
    0x31, 0xc0,                   // xor    %eax,%eax
    0xc3,                         // ret
};

DECL_SECTION_TEXT unsigned char fbrs_switch_context_code[] = {
    0x4c, 0x8b, 0x04, 0x24,                   //  mov    (%rsp),%r8
    0x4c, 0x89, 0x01,                         //  mov    %r8,(%rcx)
    0x4c, 0x8d, 0x44, 0x24, 0x08,             //  lea    0x8(%rsp),%r8
    0x4c, 0x89, 0x41, 0x08,                   //  mov    %r8,0x8(%rcx)
    0x48, 0x89, 0x59, 0x10,                   //  mov    %rbx,0x10(%rcx)
    0x48, 0x89, 0x69, 0x18,                   //  mov    %rbp,0x18(%rcx)
    0x48, 0x89, 0x71, 0x20,                   //  mov    %rsi,0x20(%rcx)
    0x48, 0x89, 0x79, 0x28,                   //  mov    %rdi,0x28(%rcx)
    0x4c, 0x89, 0x61, 0x30,                   //  mov    %r12,0x30(%rcx)
    0x4c, 0x89, 0x69, 0x38,                   //  mov    %r13,0x38(%rcx)
    0x4c, 0x89, 0x71, 0x40,                   //  mov    %r14,0x40(%rcx)
    0x4c, 0x89, 0x79, 0x48,                   //  mov    %r15,0x48(%rcx)
    0x0f, 0x11, 0x71, 0x50,                   //  movups %xmm6,0x50(%rcx)
    0x0f, 0x11, 0x79, 0x60,                   //  movups %xmm7,0x60(%rcx)
    0x44, 0x0f, 0x11, 0x41, 0x70,             //    movups %xmm8,0x70(%rcx)
    0x44, 0x0f, 0x11, 0x89, 0x80, 0x00, 0x00, //   movups %xmm9,0x80(%rcx)
    0x00, 0x44, 0x0f, 0x11, 0x91, 0x90, 0x00,
    0x00, //   movups %xmm10,0x90(%rcx)
    0x00, 0x44, 0x0f, 0x11, 0x99, 0xa0, 0x00,
    0x00, //   movups %xmm11,0xa0(%rcx)
    0x00, 0x44, 0x0f, 0x11, 0xa1, 0xb0, 0x00,
    0x00, //   movups %xmm12,0xb0(%rcx)
    0x00, 0x44, 0x0f, 0x11, 0xa9, 0xc0, 0x00,
    0x00, //   movups %xmm13,0xc0(%rcx)
    0x00, 0x44, 0x0f, 0x11, 0xb1, 0xd0, 0x00,
    0x00, //   movups %xmm14,0xd0(%rcx)
    0x00, 0x44, 0x0f, 0x11, 0xb9, 0xe0, 0x00,
    0x00,                                     //   movups %xmm15,0xe0(%rcx)
    0x00, 0x4c, 0x8b, 0x02,                   //    mov    (%rdx),%r8
    0x48, 0x8b, 0x62, 0x08,                   //    mov    0x8(%rdx),%rsp
    0x48, 0x8b, 0x5a, 0x10,                   //    mov    0x10(%rdx),%rbx
    0x48, 0x8b, 0x6a, 0x18,                   //    mov    0x18(%rdx),%rbp
    0x48, 0x8b, 0x72, 0x20,                   //    mov    0x20(%rdx),%rsi
    0x48, 0x8b, 0x7a, 0x28,                   //    mov    0x28(%rdx),%rdi
    0x4c, 0x8b, 0x62, 0x30,                   //    mov    0x30(%rdx),%r12
    0x4c, 0x8b, 0x6a, 0x38,                   //    mov    0x38(%rdx),%r13
    0x4c, 0x8b, 0x72, 0x40,                   //    mov    0x40(%rdx),%r14
    0x4c, 0x8b, 0x7a, 0x48,                   //    mov    0x48(%rdx),%r15
    0x0f, 0x10, 0x72, 0x50,                   //    movups 0x50(%rdx),%xmm6
    0x0f, 0x10, 0x7a, 0x60,                   //    movups 0x60(%rdx),%xmm7
    0x44, 0x0f, 0x10, 0x42, 0x70,             //   movups 0x70(%rdx),%xmm8
    0x44, 0x0f, 0x10, 0x8a, 0x80, 0x00, 0x00, //  movups 0x80(%rdx),%xmm9
    0x00, 0x44, 0x0f, 0x10, 0x92, 0x90, 0x00,
    0x00, //  movups 0x90(%rdx),%xmm10
    0x00, 0x44, 0x0f, 0x10, 0x9a, 0xa0, 0x00,
    0x00, //  movups 0xa0(%rdx),%xmm11
    0x00, 0x44, 0x0f, 0x10, 0xa2, 0xb0, 0x00,
    0x00, //  movups 0xb0(%rdx),%xmm12
    0x00, 0x44, 0x0f, 0x10, 0xaa, 0xc0, 0x00,
    0x00, //  movups 0xc0(%rdx),%xmm13
    0x00, 0x44, 0x0f, 0x10, 0xb2, 0xd0, 0x00,
    0x00, //  movups 0xd0(%rdx),%xmm14
    0x00, 0x44, 0x0f, 0x10, 0xba, 0xe0, 0x00,
    0x00,             //  movups 0xe0(%rdx),%xmm15
    0x00, 0x41, 0x50, //     push   %r8
    0x31, 0xc0,       //     xor    %eax,%eax
    0xc3,             //   ret
};

#endif // ABI_WIN64

#endif // CPU_X64

#ifdef CPU_AARCH64

DECL_SECTION_TEXT unsigned int fbrs_save_context_code[] = {
    0xf900001e, // str     x30, [x0]
    0x910003e9, // mov     x9, sp
    0xf9000409, // str     x9, [x0, #8]
    0xa9015013, // stp     x19, x20, [x0, #16]
    0xa9025815, // stp     x21, x22, [x0, #32]
    0xa9036017, // stp     x23, x24, [x0, #48]
    0xa9046819, // stp     x25, x26, [x0, #64]
    0xa905701b, // stp     x27, x28, [x0, #80]
    0xf9002c1d, // str     x29, [x0, #88]
    0x6d062408, // stp     d8, d9, [x0, #96]
    0x6d072c0a, // stp     d10, d11, [x0, #112]
    0x6d08340c, // stp     d12, d13, [x0, #128]
    0x6d093c0e, // stp     d14, d15, [x0, #144]
    0xd2800000, // mov     x0, #0x0
    0xd65f03c0, // ret
};

DECL_SECTION_TEXT unsigned int fbrs_load_context_code[] = {
    0xf940001e, // ldr     x30, [x0]
    0xf9400409, // ldr     x9, [x0, #8]
    0x9100013f, // mov     sp, x9
    0xa9415013, // ldp     x19, x20, [x0, #16]
    0xa9425815, // ldp     x21, x22, [x0, #32]
    0xa9436017, // ldp     x23, x24, [x0, #48]
    0xa9446819, // ldp     x25, x26, [x0, #64]
    0xa945701b, // ldp     x27, x28, [x0, #80]
    0xf9402c1d, // ldr     x29, [x0, #88]
    0x6d462408, // ldp     d8, d9, [x0, #96]
    0x6d472c0a, // ldp     d10, d11, [x0, #112]
    0x6d48340c, // ldp     d12, d13, [x0, #128]
    0x6d493c0e, // ldp     d14, d15, [x0, #144]
    0xd2800000, // mov     x0, #0x0
    0xd65f03c0, // ret
};

DECL_SECTION_TEXT unsigned int fbrs_switch_context_code[] = {
    0xf900001e, // str     x30, [x0]
    0x910003e9, // mov     x9, sp
    0xf9000409, // str     x9, [x0, #8]
    0xa9015013, // stp     x19, x20, [x0, #16]
    0xa9025815, // stp     x21, x22, [x0, #32]
    0xa9036017, // stp     x23, x24, [x0, #48]
    0xa9046819, // stp     x25, x26, [x0, #64]
    0xa905701b, // stp     x27, x28, [x0, #80]
    0xf9002c1d, // str     x29, [x0, #88]
    0x6d062408, // stp     d8, d9, [x0, #96]
    0x6d072c0a, // stp     d10, d11, [x0, #112]
    0x6d08340c, // stp     d12, d13, [x0, #128]
    0x6d093c0e, // stp     d14, d15, [x0, #144]
    0xf940003e, // ldr     x30, [x1]
    0xf9400429, // ldr     x9, [x1, #8]
    0x9100013f, // mov     sp, x9
    0xa9415033, // ldp     x19, x20, [x1, #16]
    0xa9425835, // ldp     x21, x22, [x1, #32]
    0xa9436037, // ldp     x23, x24, [x1, #48]
    0xa9446839, // ldp     x25, x26, [x1, #64]
    0xa945703b, // ldp     x27, x28, [x1, #80]
    0xf9402c3d, // ldr     x29, [x1, #88]
    0x6d462428, // ldp     d8, d9, [x1, #96]
    0x6d472c2a, // ldp     d10, d11, [x1, #112]
    0x6d48342c, // ldp     d12, d13, [x1, #128]
    0x6d493c2e, // ldp     d14, d15, [x1, #144]
    0xd2800000, // mov     x0, #0x0
    0xd65f03c0, // ret
};

#endif // CPU_AARCH64

void *fbrs_get_stack_pointer(void *stack_memory, fbrs_ptr_t stack_size) {
  if (stack_memory == 0) {
    return 0;
  }

  fbrs_ptr_t stack_end = ((fbrs_ptr_t)stack_memory + stack_size) & -16L;
  stack_end -= 128;

#ifdef CPU_X64
  stack_end -= 8;
#endif

  return (void *)stack_end;
}

static THREAD_LOCAL struct fbrs_context_t *current_fiber = 0;

struct fbrs_context_t *fbrs_current_context() { return current_fiber; }

void fbrs_set_current_context(struct fbrs_context_t *context) {
  current_fiber = context;
}

static void (*fbrs_save_context_impl)(struct fbrs_context_t *) =
    (void (*)(struct fbrs_context_t *))fbrs_save_context_code;

static void (*fbrs_load_context_impl)(struct fbrs_context_t *) =
    (void (*)(struct fbrs_context_t *))fbrs_load_context_code;

static void (*fbrs_switch_context_impl)(struct fbrs_context_t *,
                                        struct fbrs_context_t *) =
    (void (*)(struct fbrs_context_t *,
              struct fbrs_context_t *))fbrs_switch_context_code;

void fbrs_save_context(struct fbrs_context_t *context) {
  fbrs_save_context_impl(context);
}

void fbrs_load_context(struct fbrs_context_t *context) {
  // struct fbrs_context_t *old_context = fbrs_current_context();
  // fbrs_set_current_context(context);
  fbrs_load_context_impl(context);
  // fbrs_set_current_context(old_context);
}

void fbrs_switch_context(struct fbrs_context_t *old_context,
                         struct fbrs_context_t *new_context) {
  fbrs_set_current_context(new_context);
  fbrs_switch_context_impl(old_context, new_context);
  fbrs_set_current_context(old_context);
}

#endif
