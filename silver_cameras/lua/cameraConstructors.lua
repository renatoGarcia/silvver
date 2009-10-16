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

function __getMandatory(value)
    if value then
        return value
    else
        error()
    end
end

function Dragonfly(parameters)
    camera = {}
    camera.__name = 'dc1394'

    camera.uid = __getMandatory(parameters.uid)

    camera.focal_length = __getMandatory(parameters.focal_length)
    camera.principal_point = __getMandatory(parameters.principal_point)
    camera.radial_coef = __getMandatory(parameters.radial_coef)
    camera.tangential_coef = __getMandatory(parameters.tangential_coef)

    camera.translation_vector = __getOptional(parameters.translation_vector,
                                              {0, 0, 0})
    camera.rotation_matrix = __getOptional(parameters.rotation_matrix,
                                           {1, 0, 0,
                                            0, 1, 0,
                                            0, 0, 1})

    camera.resolution = __getOptional(parameters.resolution, {640, 480})
    camera.frame_rate = __getOptional(parameters.frame_rate, 30)

    return camera
end
