#!/bin/bash
case "$1" in
    linux|l)
        cp z_archive/Makefile.linux Makefile
        echo "✓ Switched to Linux build configuration"
        ;;
    windows|w)
        cp z_archive/Makefile.windows Makefile
        echo "✓ Switched to Windows build configuration"
        ;;
    *)
        echo "Usage: $0 {linux|windows|l|w}"
        echo ""
        echo "Available configurations:"
        echo "  linux, l  - Switch to Linux build"
        echo "  windows, w - Switch to Windows build"
        echo ""
        if [ -f Makefile ]; then
            echo "Current configuration:"
            if grep -q "\.exe" Makefile 2>/dev/null; then
                echo "  → Windows (targets .exe)"
            elif grep -q "pkg-config.*sdl" Makefile 2>/dev/null; then
                echo "  → Linux (uses pkg-config)"
            else
                echo "  → Unknown/Custom"
            fi
        else
            echo "No Makefile found"
        fi
        ;;
esac