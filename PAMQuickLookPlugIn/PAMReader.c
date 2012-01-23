//
//  PAMReader.c
//  PAMQuickLookPlugIn
//
//  Created by Anton Dukhovnikov on 21/01/12.
//  Copyright (c) 2012 Anton Dukhovnikov All rights reserved.
//

#include "PAMReader.h"
#include <stdio.h>

const int buffer_length = 24;

// Local functions declarations:
bool get_token(FILE *file, char *buffer);
bool read_header(FILE *file, size_t *width, size_t *height, size_t *depth, size_t *maxval, bool *ascii);
size_t get_value(FILE *file, bool ascii, size_t max_val);




bool get_token(FILE *file, char *buffer)
{
    // Machine states:
    // 0 - start or whitespace
    // 1 - comment
    // 2 - valid symbol
    
    int state = 0;
    char *current = buffer;
    
    while (1)
    {
        fread(current, 1, 1, file);
        
        switch (state)
        {
            case 0:
                if (isspace(*current)) state = 0;
                else if (*current == '#') state = 1;
                else { state = 2; current++; }
                break;
            case 1:
                if (*current == '\n') state = 0;
                break;
            case 2:
                if (isspace(*current)) { *current = 0; return true; }
                else if (++current - buffer > buffer_length) return false; // Buffer overrun.
                break;
            default:
                break;
        }
    }
    return false;
}

bool read_header(FILE *file, size_t *width, size_t *height, size_t *depth, size_t *maxval, bool *ascii)
{
    char buffer[buffer_length];
    
    if (!get_token(file, buffer)) return false;
    if (buffer[0] != 'P') return false;
    if (buffer[2] !=   0) return false;
    
    if (buffer[1] == '7')
    {
        // PAM file
        // Machine states:
        // 0 - start
        // 1 - width
        // 2 - height
        // 3 - depth
        // 4 - maxval
        // 5 - tupltype
        // 6 - end
        
        *ascii = false;
        int state = 0;
        while (state < 6)
        {
            if (get_token(file, buffer))
            {
                switch (state) 
                {
                    case 0:
                        if      (!strncmp(buffer,    "WIDTH", buffer_length)) state = 1;
                        else if (!strncmp(buffer,   "HEIGHT", buffer_length)) state = 2;
                        else if (!strncmp(buffer,    "DEPTH", buffer_length)) state = 3;
                        else if (!strncmp(buffer,   "MAXVAL", buffer_length)) state = 4;
                        else if (!strncmp(buffer, "TUPLTYPE", buffer_length)) state = 5;
                        else if (!strncmp(buffer,   "ENDHDR", buffer_length)) state = 6;
                        else return false;
                        break;
                    case 1:
                        sscanf(buffer, "%lu", width);
                        state = 0;
                        break;
                    case 2:
                        sscanf(buffer, "%lu", height);
                        state = 0;
                        break;
                    case 3:
                        sscanf(buffer, "%lu", depth);
                        state = 0;
                        break;
                    case 4:
                        sscanf(buffer, "%lu", maxval);
                        state = 0;
                        break;
                    case 5:
                        if (!strncmp(buffer, "BLACKANDWHITE", buffer_length))
                        {
                            return false; 
                        }
                        else if (!strncmp(buffer, "BLACKANDWHITE_ALPHA", buffer_length))
                        {
                            return false;    
                        }
                        else if (!strncmp(buffer, "GRAYSCALE", buffer_length))
                        {
                            
                        }
                        else if (!strncmp(buffer, "GRAYSCALE_ALPHA", buffer_length))
                        {
                            
                        }
                        else if (!strncmp(buffer, "RGB", buffer_length))
                        {
                            
                        }
                        else if (!strncmp(buffer, "RGB_ALPHA", buffer_length))
                        {
                            
                        }
                        state = 0;
                        break;
                        
                    default:
                        break;
                }
            }
        }
        return true;
    }
    
    // PBM, PGM or PPM file
    
    switch (buffer[1]) 
    {
        case '1':
            *ascii = true;
            *depth = 1;
            *maxval = 1;
            break;
        case '2':
            *ascii = true;
            *depth = 1;
            *maxval = 0;
            break;
        case '3':
            *ascii = true;
            *depth = 3;
            *maxval = 0;
            break;
//        case '4':
//            *ascii = false;
//            *depth = 1;
//            *maxval = 1;
//            break;
        case '5':
            *ascii = false;
            *depth = 1;
            *maxval = 0;
            break;
        case '6':
            *ascii = false;
            *depth = 3;
            *maxval = 0;
            break;
        default:
            return false;
    }
    
    if (!get_token(file, buffer)) return false;
    if (!sscanf(buffer, "%lu", width)) return false;
    
    if (!get_token(file, buffer)) return false;
    if (!sscanf(buffer, "%lu", height)) return false;
    
    if (*maxval) return true;
    
    if (!get_token(file, buffer)) return false;
    if (!sscanf(buffer, "%lu", maxval)) return false;
    
    return true;
}

size_t get_value(FILE *file, bool ascii, size_t max_val)
{
    size_t value;
    
    if (ascii)
    {
        fscanf(file, "%lu", &value);
    }
    else
    {
        if (max_val < 256) 
        {
            value = fgetc(file);
        }
        else
        {
            value = fgetc(file) * 256;
            fseek(file, SEEK_CUR, 1);
        }
    }
    
    return value;
}



CGImageRef CreateImageWithURL(CFURLRef url)
{
    char path[MAXPATHLEN];
    if (!CFURLGetFileSystemRepresentation(url, 0, (UInt8 *)path, MAXPATHLEN)) return NULL;
    
    FILE *file = fopen(path, "r");
    if (!file) return NULL;
    
    char *buffer;
    bool ascii;
    size_t width;
    size_t height;
    size_t depth;
    size_t target_components;
    size_t max_val;
    
    CFStringRef colorSpaceName;
    int alpha;
    
    if (!read_header(file, &width, &height, &depth, &max_val, &ascii)) return NULL;

    switch (depth) 
    {
        case 1:
            target_components = 1;
            colorSpaceName = kCGColorSpaceGenericGray;
            alpha = kCGImageAlphaNone;
            break;
        case 2:
            target_components = 2;
            colorSpaceName = kCGColorSpaceGenericGray;
            alpha = kCGImageAlphaLast;
            break;   
        case 3:
            target_components = 4;
            colorSpaceName = kCGColorSpaceGenericRGB;
            alpha = kCGImageAlphaNoneSkipLast;
            break;   
        case 4:
            target_components = 4;
            colorSpaceName = kCGColorSpaceGenericRGB;
            alpha = kCGImageAlphaLast;
            break;   
        default:
            return NULL;
    }
    
    size_t buffer_length = width * height * target_components;
    buffer = (char *)malloc(buffer_length);
    size_t current = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            switch (depth) 
            {
                case 1:
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    break;
                case 2:
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    break;
                case 3:
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    buffer[current++] = 255;
                    break;
                case 4:
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    buffer[current++] = get_value(file, ascii, max_val) * 255 / max_val;
                    break;
                default:
                    break;
            }
        }
    }

    CGColorSpaceRef colorSpace=CGColorSpaceCreateWithName(colorSpaceName);
    CGContextRef bitmapContext=CGBitmapContextCreate(buffer, width, height, 8, target_components * width, colorSpace, alpha);
    CFRelease(colorSpace);
    
    CGImageRef image = CGBitmapContextCreateImage(bitmapContext);
    CGContextRelease(bitmapContext);
    return image;
}