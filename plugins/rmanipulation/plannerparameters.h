// -*- coding: utf-8 -*-
// Copyright (C) 2006-2010 Rosen Diankov (rosen.diankov@gmail.com)
//
// This file is part of OpenRAVE.
// OpenRAVE is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
/** \file planner.h
    \brief Planning related defintions.
*/
#ifndef OPENRAVE_PLANNER_PARAMETERS_H
#define OPENRAVE_PLANNER_PARAMETERS_H

namespace OpenRAVE {

class WorkspaceTrajectoryParameters : public PlannerBase::PlannerParameters
{
public:
 WorkspaceTrajectoryParameters(EnvironmentBasePtr penv) : _fMaxDeviationAngle(0.15*PI), _bMaintainTiming(false), _bGreedySearch(false), _bIgnoreFirstCollision(false), _fMinimumCompleteTime(1e30f), _penv(penv), _bProcessing(false) {
        _vXMLParameters.push_back("maxdeviationangle");
        _vXMLParameters.push_back("maintaintiming");
        _vXMLParameters.push_back("greedysearch");
        _vXMLParameters.push_back("ignorefirstcollision");
        _vXMLParameters.push_back("minimumcompletetime");
        _vXMLParameters.push_back("workspacetraj");
    }
    
    dReal _fMaxDeviationAngle; ///< the maximum angle the next iksolution can deviate from the expected direction computed by the jacobian 
    bool _bMaintainTiming; ///< maintain timing with input trajectory
    bool _bGreedySearch; ///< if true, will greeidly choose solutions (can possibly fail even a solution exists)
    bool _bIgnoreFirstCollision; ///< if true, will allow the robot to be in environment collision for the initial part of the trajectory. Once the robot gets out of collision, it will execute its normal following phase until it gets into collision again. This option is used when lifting objects from a surface, where the object is already in collision with the surface.
    dReal _fMinimumCompleteTime; ///< specifies the minimum trajectory that must be followed for planner to declare success. If 0, then the entire trajectory has to be followed.
    TrajectoryBasePtr _workspacetraj; ///< workspace trajectory
 protected:
    EnvironmentBasePtr _penv;
    bool _bProcessing;
    // save the extra data to XML
    virtual bool serialize(std::ostream& O) const
    {
        if( !PlannerParameters::serialize(O) ) {
            return false;
        }
        O << "<maxdeviationangle>" << _fMaxDeviationAngle << "</maxdeviationangle>" << endl;
        O << "<maintaintiming>" << _bMaintainTiming << "</maintaintiming>" << endl;
        O << "<greedysearch>" << _bGreedySearch << "</greedysearch>" << endl;
        O << "<ignorefirstcollision>" << _bIgnoreFirstCollision << "</ignorefirstcollision>" << endl;
        O << "<minimumcompletetime>" << _fMinimumCompleteTime << "</minimumcompletetime>" << endl;
        if( !!_workspacetraj ) {
            O << "<workspacetraj>";
            _workspacetraj->Write(O,TrajectoryBase::TO_IncludeTimestamps|TrajectoryBase::TO_IncludeBaseTransformation);
            O << "</workspacetraj>" << endl;
        }
        return !!O;
    }

    ProcessElement startElement(const std::string& name, const std::list<std::pair<std::string,std::string> >& atts)
    {
        if( _bProcessing ) {
            return PE_Ignore;
        }
        switch( PlannerBase::PlannerParameters::startElement(name,atts) ) {
        case PE_Pass: break;
        case PE_Support: return PE_Support;
        case PE_Ignore: return PE_Ignore;
        }    
        _bProcessing = name=="maxdeviationangle" || name=="maintaintiming" || name=="greedysearch" || name=="ignorefirstcollision" || name=="minimumcompletetime" || name=="workspacetraj";
        return _bProcessing ? PE_Support : PE_Pass;
    }
        
    // called at the end of every XML tag, _ss contains the data 
    virtual bool endElement(const std::string& name)
    {
        // _ss is an internal stringstream that holds the data of the tag
        if( _bProcessing ) {
            if( name == "maxdeviationangle") {
                _ss >> _fMaxDeviationAngle;
            }
            else if( name == "maintaintiming" ) {
                _ss >> _bMaintainTiming;
            }
            else if( name == "greedysearch" ) {
                _ss >> _bGreedySearch;
            }
            else if( name == "ignorefirstcollision" ) {
                _ss >> _bIgnoreFirstCollision;
            }
            else if( name == "minimumcompletetime" ) {
                _ss >> _fMinimumCompleteTime;
            }
            else if( name == "workspacetraj" ) {
                if( !_workspacetraj ) {
                    _workspacetraj = RaveCreateTrajectory(_penv,"");
                }
                _workspacetraj->Read(_ss,RobotBasePtr());
            }
            else {
                RAVELOG_WARN(str(boost::format("unknown tag %s\n")%name));
            }
            _bProcessing = false;
            return false;
        }
        // give a chance for the default parameters to get processed
        return PlannerParameters::endElement(name);
    }
};

} // end namespace OpenRAVE

#endif