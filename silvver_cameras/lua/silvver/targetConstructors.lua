-- Copyright 2010 Renato Florentino Garcia <fgar.renato@gmail.com>
--
-- This program is free software: you can redistribute it and/or modify
-- it under the terms of the GNU General Public License version 3, as
-- published by the Free Software Foundation.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program.  If not, see <http://www.gnu.org/licenses/>.

function __getOptional(value, default)
    if value then
        return value
    else
        return default
    end
end

function __transpose(matrix_3_3)
    transposed_matrix = {0,0,0,0,0,0,0,0,0}

    transposed_idx = {1,4,7,2,5,8,3,6,9}

    for i,k in ipairs(transposed_idx) do
        transposed_matrix[k] = matrix_3_3[i]
    end

    return transposed_matrix
end

function target2body(translation, rotation)
    body_rotation = __transpose(rotation)

    body_translation = {}
    body_translation[1] = -1 * (body_rotation[1]*translation[1] +
                                body_rotation[2]*translation[2] +
                                body_rotation[3]*translation[3])
    body_translation[2] = -1 * (body_rotation[4]*translation[1] +
                                body_rotation[5]*translation[2] +
                                body_rotation[6]*translation[3])
    body_translation[3] = -1 * (body_rotation[7]*translation[1] +
                                body_rotation[8]*translation[2] +
                                body_rotation[9]*translation[3])

    return {body_translation, body_rotation}
end

function ARTKP(parameters)
    if parameters.body_pose then
        parameters.body_translation = parameters.body_pose[1]
        parameters.body_rotation = parameters.body_pose[2]
        parameters.body_pose = nil
    end

    artkp = {}
    artkp.__type = 'artkp'
    artkp.pattern_width = parameters.pattern_width
    artkp.threshold = __getOptional(parameters.threshold, 100)
    artkp.body_translation = __getOptional(parameters.body_translation,
                                           {0, 0, 0})
    artkp.body_rotation = __getOptional(parameters.body_rotation,
                                        {1, 0, 0,
                                         0, 1, 0,
                                         0, 0, 1})

    for idx,value in ipairs(parameters) do
        artkp[idx] = value
    end

    return artkp
end