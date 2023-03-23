# Distance away from the FBEWMA that data should be removed.
DELTA = 10

# clip data above this value:
HIGH_CLIP = 90

# clip data below this value:
LOW_CLIP = -1

# random values above this trigger a spike:
RAND_HIGH = 80

# random values below this trigger a negative spike:
RAND_LOW = -1

# How many samples to run the FBEWMA over.
SPAN = 500

# spike amplitude
SPIKE = 100

def create_sample_data():
    ''' Create sine wave, amplitude +/-2 with random spikes. '''
    x = np.linspace(0, 2*np.pi, 1000)
    y = 2 * np.sin(x)
    df = pd.DataFrame(list(zip(x,y)), columns=['x', 'y'])
    df['rand'] = np.random.random_sample(len(x),)
    # create random positive and negative spikes
    cond_spike_high = (df['rand'] > RAND_HIGH)
    df['spike_high'] = np.where(cond_spike_high, SPIKE, 0)
    cond_spike_low = (df['rand'] < RAND_LOW)
    df['spike_low'] = np.where(cond_spike_low, -SPIKE, 0)
    df['y_spikey'] = df['y'] + df['spike_high'] + df['spike_low']
    return df

def clip_data(unclipped, high_clip, low_clip):
    ''' Clip unclipped between high_clip and low_clip. 
    unclipped contains a single column of unclipped data.'''
    
    # convert to np.array to access the np.where method
    # clip data above HIGH_CLIP or below LOW_CLIP
    cond_high_clip = (unclipped > HIGH_CLIP) | (unclipped < LOW_CLIP)
    np_clipped = np.where(cond_high_clip, np.nan, unclipped).ravel()
    np_clipped = pd.DataFrame(np_clipped)
    return np_clipped


def ewma_fb(df_column, span):
    ''' Apply forwards, backwards exponential weighted moving average (EWMA) to df_column. '''
    # Forwards EWMA.
    fwd = pd.Series.ewm(df_column, span=span).mean()
    # Backwards EWMA.
    bwd = pd.Series.ewm(df_column[::-1],span=10).mean()
    # Add and take the mean of the forwards and backwards EWMA.
    stacked_ewma = np.vstack(( fwd, bwd[::-1] ))
    fb_ewma = np.mean(stacked_ewma, axis=0)
    return fb_ewma
    
    
def remove_outliers(spikey, fbewma, delta):
    ''' Remove data from df_spikey that is > delta from fbewma. '''
    np_spikey = np.array(spikey)
    np_fbewma = np.array(fbewma)
    cond_delta = (np.abs(np_spikey-np_fbewma) > delta)
    np_remove_outliers = np.where(cond_delta, np.nan, np_spikey)
    return np_remove_outliers


df_clipped = clip_data(df, HIGH_CLIP, LOW_CLIP)
df_ewma_fb = pd.DataFrame(ewma_fb(df_clipped, SPAN).ravel())
df_remove_outliers = pd.DataFrame(remove_outliers(df_clipped, df_ewma_fb, DELTA).ravel())
print(df_remove_outliers)
df_interpolated = df_remove_outliers.interpolate()
    
#ax = df.plot(x='x', y='y_spikey', color='blue', alpha=0.5)
#ax2 = df.plot(x='x', y=df_interpolated, color='black', ax=ax)