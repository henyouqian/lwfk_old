//
//  Shader.fsh
//  dev
//
//  Created by Wei Li on 13-6-1.
//  Copyright (c) 2013年 Wei Li. All rights reserved.
//

varying lowp vec4 colorVarying;

void main()
{
    gl_FragColor = colorVarying;
}
