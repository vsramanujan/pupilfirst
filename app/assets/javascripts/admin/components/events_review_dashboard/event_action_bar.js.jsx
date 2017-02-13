class EventsReviewDashboardEventActionBar extends React.Component {
  constructor(props) {
    super(props);
    this.state = {status: null, grade: null, statusMissing: false};
    this.statusChange = this.statusChange.bind(this);
    this.gradeChange = this.gradeChange.bind(this);
    this.saveReview = this.saveReview.bind(this);
    this.radioInputId = this.radioInputId.bind(this);
    this.radioInputName = this.radioInputName.bind(this);
  }

  statusChange(event) {
    this.setState({status: event.target.value});
  }

  gradeChange(event) {
    this.setState({grade: event.target.value});
  }

  saveReview(event) {
    console.log('Saving Review...');
    if (this.state.status == null) {
      this.setState({statusMissing: true});
    } else {
      this.setState({statusMissing: false});
      let eventId = this.props.eventId;
      let status = this.state.status;
      let grade = this.state.grade;
      let removeEvent = this.props.removeEventCB;
      let postUrl = '/admin/timeline_events/' + eventId + '/quick_review';
      $.post({
        url: postUrl,
        data: {status: status, grade: grade},
        success: function () {
          console.log('Event was successfully marked ' + status);
          new PNotify({
            title: 'Event Reviewed',
            text: 'Event ' + eventId + ' marked ' + status
          });
          removeEvent(eventId);
        },
        beforeSend: function () {
          event.target.innerHTML = 'Recording Review...'
        },
        error: function () {
          alert('Failed to record your review. Try again')
        }
      });
    }
  }

  radioInputId(name) {
    return name + '-' + this.props.eventId;
  }

  radioInputName(name) {
    return 'event-' + this.props.eventId + '-' + name;
  }

  render() {
    return (
      <tr>
        <td colSpan='2'>
          <strong>Status:</strong>
          <br/>
          <label htmlFor={ this.radioInputId('verified') }>
            <input type='radio' id={ this.radioInputId('verified') } value='verified' name={ this.radioInputName('status') }
                   onChange={ this.statusChange }/>&nbsp;Verified&nbsp;
          </label>
          <label htmlFor={ this.radioInputId('needs_improvement') }>
            <input type='radio' id={ this.radioInputId('needs_improvement') } value='needs_improvement' name={ this.radioInputName('status') }
                   onChange={ this.statusChange }/>&nbsp;Needs Improvement&nbsp;
          </label>
          <label htmlFor={ this.radioInputId('not_accepted') }>
            <input type='radio' id={ this.radioInputId('not_accepted') } value='not_accepted' name={ this.radioInputName('status') }
                   onChange={ this.statusChange }/>&nbsp;Not Accepted&nbsp;
          </label>
        </td>
        <td>
          <strong>Grade:</strong>
          <br/>
          <label htmlFor={ this.radioInputId('wow') }>
            <input type='radio' id={this.radioInputId('wow') } value='wow' name={ this.radioInputName('grade') }
                   onChange={ this.gradeChange }/>&nbsp;Wow&nbsp;
          </label>
          <label htmlFor={ this.radioInputId('great') }>
            <input type='radio' id={ this.radioInputId('great') } value='great' name={ this.radioInputName('grade') }
                   onChange={ this.gradeChange }/>&nbsp;
            Great&nbsp;
          </label>
          <label htmlFor={ this.radioInputId('good') }>
            <input type='radio' id={ this.radioInputId('good') } value='good' name={ this.radioInputName('grade') }
                   onChange={ this.gradeChange }/>&nbsp;
            Good&nbsp;
          </label>
        </td>
        <td>
          <a className='button' onClick={ this.saveReview }>Save Review</a>
          { this.state.statusMissing &&
          <div style={{color: 'red'}}>Select a status first!</div>
          }
        </td>
      </tr>
    )
  }
}
;

EventsReviewDashboardEventActionBar.propTypes = {
  eventId: React.PropTypes.number,
  removeEventCB: React.PropTypes.func
};