int RGB2Hex(int r, int g, int b) {
    return (r << 16) + (g << 8) + b;
}

struct Color {
    int r;
    int g;
    int b;
};

int Color2Hex(Color color) {
    return (color.r << 16) + (color.g << 8) + color.b;
} 
