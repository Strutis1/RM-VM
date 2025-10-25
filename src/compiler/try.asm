
#example comment for example program

@data:
    ans db 'y'
end data;

start:
    mov RAX, 2;
    mov RBX, 1;
    add RBX, RAX;
    int 21;
end start;

#comments