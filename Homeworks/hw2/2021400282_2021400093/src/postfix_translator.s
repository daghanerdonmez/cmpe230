.section .bss
input_buffer:
    .space 512
output_buffer:
    .space 256

.section .text
.global _start

_start:
    # Read input from user
    mov $0, %eax          # syscall number for sys_read
    mov $0, %edi          # file descriptor (stdin)
    lea input_buffer(%rip), %rsi  # buffer to store input
    mov $512, %edx         # maximum number of bytes to read
    syscall

    mov $0, %r10 #Set counter to 0 initially.


    lea output_buffer(%rip), %r15
    lea input_buffer(%rip), %r14 

# Beginning of parsing an input

parse_loop:
    mov $0, %rbx
    movb (%r14), %bl  # Load next character

    # Check if the char is null terminator
    cmp $'\n', %rbx
    je end_parsing

    # Check if it is whitespace
    cmp $0x20, %bl
    je next_char

    cmp $0x2a, %bl
    je perform_mul

    cmp $0x2b, %bl
    je perform_addition

    cmp $0x2d, %bl
    je perform_sub

    cmp $0x7c, %bl
    je perform_or

    cmp $0x26, %bl
    je perform_and

    cmp $0x5e, %bl
    je perform_xor

    jmp parse_token

next_char:
    inc %r14
    jmp parse_loop

end_parsing:
    jmp print_output

# Beginning of parsing a new integer.
parse_token:
    mov $0, %rcx          # Initialize token value to 0

token_loop:
    mov $0, %rbx
    movb (%r14), %bl  # Load next character
    cmp $0x20, %bl      # Check for space
    je end_parse_token   # If space, end of token

    sub $0x30, %rbx      # Convert character to digit
    imul $10, %rcx       # Multiply token by 10
    add %rbx, %rcx       # Add current character to token

    inc %r14             # Move to next character
    jmp token_loop

end_parse_token:
    push %rcx            # Push token onto the stack
    inc %r14
    jmp parse_loop

# Function to perform addition
perform_addition:
    pop %r13           # Pop x2 from the stack
    pop %r12             # Pop x1 from the stack
    

    mov $2048, %rax
    call bloop2
    call add_x2

    mov $2048, %rax
    call bloop1
    call add_x1

    call add_addition_code

    add %r13, %r12       # Add x2 to x1
    push %r12            # Push the result back onto the stack
    
    inc %r14
    jmp parse_loop

# Function to perform multiplication
perform_mul:
    pop %r13
    pop %r12
    
    mov $2048, %rax
    call bloop2
    call add_x2
    
    mov $2048, %rax
    call bloop1
    call add_x1

    call add_mul_code

    imul %r13, %r12
    push %r12

    inc %r14
    jmp parse_loop

# Function to perform subtraction
perform_sub:
    pop %r13
    pop %r12

    mov $2048, %rax
    call bloop2
    call add_x2

    mov $2048, %rax
    call bloop1
    call add_x1

    call add_sub_code

    sub %r13, %r12
    push %r12
    
    inc %r14
    jmp parse_loop

# Function to perform xor
perform_xor:
    pop %r13
    pop %r12

    mov $2048, %rax
    call bloop2
    call add_x2

    mov $2048, %rax
    call bloop1
    call add_x1

    call add_xor_code

    xorq %r13, %r12
    push %r12

    inc %r14
    jmp parse_loop

# Function to perform or
perform_or:
    pop %r13
    pop %r12

    mov $2048, %rax
    call bloop2
    call add_x2

    mov $2048, %rax
    call bloop1
    call add_x1

    call add_or_code

    orq %r13, %r12
    push %r12

    inc %r14
    jmp parse_loop

# Function to perform and
perform_and:
    pop %r13
    pop %r12

    mov $2048, %rax
    call bloop2
    call add_x2

    mov $2048, %rax
    call bloop1
    call add_x1

    call add_and_code

    and %r13, %r12
    push %r12

    inc %r14
    jmp parse_loop

bloop1:
    cmp $0, %rax
    je end_bloop1

    mov %rax, %r9

    and %r12, %r9

    cmp $0, %r9
    je add_zero1

    cmp %rax, %r9
    je add_one1

bloop2:
    cmp $0, %rax
    je end_bloop2

    mov %rax, %r9

    and %r13, %r9

    cmp $0, %r9
    je add_zero2

    cmp %rax, %r9
    je add_one2

add_zero1:
    movb $'0', (%r15)
    inc %r15
    inc %r10 
    shrq $1, %rax

    jmp bloop1

add_one1:
    movb $'1', (%r15)
    inc %r15
    inc %r10

    shrq $1, %rax

    jmp bloop1

add_zero2:
    movb $'0', (%r15)
    inc %r15
    inc %r10

    shrq $1, %rax

    jmp bloop2

add_one2:
    movb $'1', (%r15)
    inc %r15
    inc %r10

    shrq $1, %rax

    jmp bloop2

end_bloop1:
    movb $0x20, (%r15)
    inc %r15
    inc %r10
    ret

end_bloop2:
    movb $0x20, (%r15)
    inc %r15
    inc %r10
    ret


add_x2:
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $0x0A, (%r15)
    inc %r15
    inc %r10

    ret
    

add_x1:
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $0x0A, (%r15)
    inc %r15
    inc %r10

    ret

add_addition_code:
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    jmp add_operation_codes

add_mul_code:
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    jmp add_operation_codes

add_sub_code:
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    jmp add_operation_codes

add_xor_code:
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    jmp add_operation_codes

add_and_code:
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    jmp add_operation_codes

add_or_code:
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    jmp add_operation_codes

add_operation_codes:
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $0x20, (%r15)
    inc %r15
    inc %r10

    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'0', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $'1', (%r15)
    inc %r15
    inc %r10
    movb $0x0A, (%r15)
    inc %r15
    inc %r10

    ret       


print_output:
    lea output_buffer(%rip), %rsi  
    mov $1, %rax  
    mov $1, %rdi  
    mov %r10, %rdx  
    syscall

exit_program:    
    # Exit program
    mov $60, %rax         # syscall number for sys_exit
    xor %rdi, %rdi        # Return code 0
    syscall
