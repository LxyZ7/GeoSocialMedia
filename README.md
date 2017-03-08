# GeoSocialMedia

# List of function names and descriptions:

/***************************************************************
* Estimating home location of users:
* Return value: pair<double, double>, the latitude and 
* the longitude of the user we want to estimate;
* Input: uid, the id of the user we want to estimate
* network, the undiredted connections between user ids
* userlist, locationlist, a vector of user ids and their
* home location.
***************************************************************/
pair<double, double> homeLocation(string uid, vector<pair<string, string>>& network, vector<string> userlist, 
	vector<pair<double, double>> locationlist);


/***************************************************************
* Estimating home location of a list of users:
* Return value: vector<pair<double, double>>, the list 
* of latitude and longitude of the users we want to estimate;
* Input: list of uids, the ids of the users we want to estimate
* network, the undiredted connections between user ids
* userlist, locationlist, a vector of user ids and their
* home location.
***************************************************************/
vector<pair<double, double>> locationEstimation(vector<string> estimateuid, 
	vector<pair<string, string>>& network, vector<string> userlist, vector<pair<double, double>> locationlist);

/***************************************************************
* Estimating geotag of tweets:
* lib/hmmbased.cpp
***************************************************************/