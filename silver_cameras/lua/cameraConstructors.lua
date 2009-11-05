-- Copyright 2009 Renato Florentino Garcia <fgar.renato@gmail.com>
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

function Dragonfly(parameters)
    camera = {}
    camera.__name = 'dc1394'

    camera.uid = parameters.uid

    camera.focal_length = parameters.focal_length
    camera.principal_point = parameters.principal_point
    camera.radial_coef = parameters.radial_coef
    camera.tangential_coef = parameters.tangential_coef

    camera.translation_vector = __getOptional(parameters.translation_vector,
                                              {0, 0, 0})
    camera.rotation_matrix = __getOptional(parameters.rotation_matrix,
                                           {1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1})

    camera.resolution = __getOptional(parameters.resolution, {640, 480})
    camera.frame_rate = __getOptional(parameters.frame_rate, 30)

    camera.color_mode = __getOptional(parameters.color_mode, 'mono8')
    camera.brightness = __getOptional(parameters.brightness, 'untouched')
    camera.exposure = __getOptional(parameters.exposure, 'untouched')
    camera.white_balance = __getOptional(parameters.white_balance,
                                         {'untouched', 'untouched'})
    camera.shutter = __getOptional(parameters.shutter, 'untouched')
    camera.gain = __getOptional(parameters.gain, 'untouched')

    return camera
end
