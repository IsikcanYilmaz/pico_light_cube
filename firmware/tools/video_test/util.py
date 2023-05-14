#!/usr/bin/env python3

class Bitmap:
    def __init__(self, w, h):
        self.w = w
        self.h = h
        self.bytes = bytearray(int(1+(w*h/8)))

    def get_pix_idx(self, x, y):
        return int((self.w * y) + x)

    def get_byte_idx(self, x, y):
        return int(self.get_pix_idx(x, y) / 8)

    def get_bit_idx(self, x, y):
        return int(self.get_pix_idx(x, y) % 8)
    
    def get_pixel(self, x, y):
        byte_idx = self.get_byte_idx(x, y)
        bit_idx = self.get_bit_idx(x, y)
        return (self.bytes[byte_idx] & (1 << bit_idx)) > 0

    def set_pixel(self, x, y, val):
        byte_idx = self.get_byte_idx(x, y)
        bit_idx = self.get_bit_idx(x, y)
        if (val):
            self.bytes[byte_idx] = self.bytes[byte_idx] | (1 << bit_idx)
        else:
            self.bytes[byte_idx] = self.bytes[byte_idx] & ~(1 << bit_idx)

    def get_bytes(self):
        return self.bytes
