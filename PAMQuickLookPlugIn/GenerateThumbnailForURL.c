//
//  GenerateThumbnailForURL.c
//  PAMQuickLookPlugIn
//
//  Created by Anton Dukhovnikov on 21/01/12.
//  Copyright (c) 2012 Anton Dukhovnikov All rights reserved.
//

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include <QuickLook/QuickLook.h>
#include "PAMReader.h"

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize);
void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail);

/* -----------------------------------------------------------------------------
    Generate a thumbnail for file

   This function's job is to create thumbnail for designated file as fast as possible
   ----------------------------------------------------------------------------- */

OSStatus GenerateThumbnailForURL(void *thisInterface, QLThumbnailRequestRef thumbnail, CFURLRef url, CFStringRef contentTypeUTI, CFDictionaryRef options, CGSize maxSize)
{
    CGImageRef image = CreateImageWithURL(url);
    if (!image) return noErr;
    
    size_t width = CGImageGetWidth(image);
    size_t height = CGImageGetHeight(image);
    CGSize size = CGSizeMake(width, height);
    
    CGContextRef cgContext = QLThumbnailRequestCreateContext(thumbnail, size, true, options);
    if(cgContext) 
    {
        CGContextDrawImage(cgContext, CGRectMake(0, 0, width, height), image);
        QLThumbnailRequestFlushContext(thumbnail, cgContext);        
        CFRelease(cgContext);
    }
    
    CGImageRelease(image);
    return noErr;
}

void CancelThumbnailGeneration(void *thisInterface, QLThumbnailRequestRef thumbnail)
{
    // Implement only if supported
}
