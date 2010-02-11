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

function ARTKP(parameters)
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